#include "infrastructure/collectors/vram/repositorio_feedback_cifrado.h"

#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/entidades/entrada_feedback.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "infrastructure/security/arquivo_chave_local.h"
#include "infrastructure/security/openssl_vault.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QString>
#include <QTemporaryDir>
#include <QTimeZone>

#include <gtest/gtest.h>

#include <memory>

using my_comp::infrastructure::security::ArquivoChaveLocal;
using my_comp::infrastructure::security::OpenSslVault;
using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::EntradaFeedback;
using my_comp::infrastructure::vram::RepositorioFeedbackCifrado;

namespace {

const QDateTime k_t0 = QDateTime::fromSecsSinceEpoch(1'700'000'000, QTimeZone::utc());

EntradaFeedback feedback(const QString& binario, Categoria veredito, const QString& motivo) {
    return EntradaFeedback{
        .id = 0,
        .chave = ChaveProcesso::de_binario(binario),
        .veredito = veredito,
        .motivo = motivo,
        .criado_em = k_t0,
        .visto_em = k_t0,
        .revisado_em = std::nullopt,
    };
}

std::unique_ptr<RepositorioFeedbackCifrado> repo_em(const QString& dir) {
    return std::make_unique<RepositorioFeedbackCifrado>(std::make_shared<OpenSslVault>(), dir);
}

QString caminho_dados(const QString& dir) { return dir + QStringLiteral("/vram_feedback.bin"); }
QString caminho_chave(const QString& dir) { return dir + QStringLiteral("/vram_feedback.key"); }

QByteArray ler_tudo(const QString& caminho) {
    QFile f{caminho};
    return f.open(QIODevice::ReadOnly) ? f.readAll() : QByteArray{};
}

// Grava um plaintext arbitrário no formato do repositório (cabeçalho + iv + tag + ct),
// para exercitar a validação do conteúdo decifrado.
void gravar_plaintext_cifrado(const QString& dir, const QByteArray& json) {
    const auto chave = ArquivoChaveLocal{caminho_chave(dir)}.ler_ou_criar();
    ASSERT_TRUE(chave.has_value());
    OpenSslVault vault;
    const std::vector<std::uint8_t> pt(json.begin(), json.end());
    const auto dados = vault.encrypt(pt, *chave);
    QByteArray bin = RepositorioFeedbackCifrado::cabecalho();
    bin.append(reinterpret_cast<const char*>(dados.iv.data()), static_cast<qsizetype>(dados.iv.size()));
    bin.append(reinterpret_cast<const char*>(dados.tag.data()), static_cast<qsizetype>(dados.tag.size()));
    bin.append(reinterpret_cast<const char*>(dados.ciphertext.data()),
               static_cast<qsizetype>(dados.ciphertext.size()));
    QFile f{caminho_dados(dir)};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write(bin);
}

}

TEST(RepositorioFeedbackCifradoTest, ConstrutorNaoTocaODisco) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString dir = tmp.path() + QStringLiteral("/sub");
    auto repo = repo_em(dir);
    EXPECT_FALSE(QDir{dir}.exists());
}

TEST(RepositorioFeedbackCifradoTest, LerSemDadosNaoCriaArquivos) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    auto repo = repo_em(tmp.path());
    EXPECT_FALSE(repo->buscar(ChaveProcesso::de_binario(QStringLiteral("firefox"))).has_value());
    EXPECT_TRUE(repo->listar_todos().isEmpty());
    EXPECT_FALSE(QFile::exists(caminho_dados(tmp.path())));
    EXPECT_FALSE(QFile::exists(caminho_chave(tmp.path())));
}

TEST(RepositorioFeedbackCifradoTest, InserirEBuscarAtribuiIdSequencial) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    auto repo = repo_em(tmp.path());
    repo->inserir(feedback(QStringLiteral("firefox"), Categoria::PrecisaGpu, QStringLiteral("WebGL")));
    repo->inserir(feedback(QStringLiteral("slack"), Categoria::CandidatoObvioCpu, QStringLiteral("chat")));

    const auto f = repo->buscar(ChaveProcesso::de_binario(QStringLiteral("firefox")));
    ASSERT_TRUE(f.has_value());
    EXPECT_EQ(f->id, 1);
    EXPECT_EQ(f->veredito, Categoria::PrecisaGpu);
    EXPECT_EQ(f->motivo, QStringLiteral("WebGL"));
    const auto s = repo->buscar(ChaveProcesso::de_binario(QStringLiteral("slack")));
    ASSERT_TRUE(s.has_value());
    EXPECT_EQ(s->id, 2);
    EXPECT_FALSE(repo->buscar(ChaveProcesso::de_wm_class(QStringLiteral("firefox"))).has_value());
}

TEST(RepositorioFeedbackCifradoTest, PersisteEntreInstancias) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    repo_em(tmp.path())->inserir(
        feedback(QStringLiteral("firefox"), Categoria::PrecisaGpu, QStringLiteral("WebGL")));

    auto outra = repo_em(tmp.path());
    const auto f = outra->buscar(ChaveProcesso::de_binario(QStringLiteral("firefox")));
    ASSERT_TRUE(f.has_value());
    EXPECT_EQ(f->id, 1);
    EXPECT_EQ(f->veredito, Categoria::PrecisaGpu);
    EXPECT_EQ(f->criado_em, k_t0);
    EXPECT_FALSE(f->revisado_em.has_value());
    // O contador de ids também persiste: o próximo não reaproveita o 1.
    outra->inserir(feedback(QStringLiteral("slack"), Categoria::CandidatoObvioCpu, QString{}));
    EXPECT_EQ(outra->buscar(ChaveProcesso::de_binario(QStringLiteral("slack")))->id, 2);
}

TEST(RepositorioFeedbackCifradoTest, ArquivoNoDiscoNaoContemTextoClaroETem0600) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    repo_em(tmp.path())->inserir(feedback(QStringLiteral("binario_marcador_qwerty"),
                                          Categoria::PrecisaGpu, QStringLiteral("motivo_marcador_zxcv")));
    const auto bruto = ler_tudo(caminho_dados(tmp.path()));
    ASSERT_FALSE(bruto.isEmpty());
    EXPECT_TRUE(bruto.startsWith(RepositorioFeedbackCifrado::cabecalho()));
    EXPECT_FALSE(bruto.contains("binario_marcador_qwerty"));
    EXPECT_FALSE(bruto.contains("motivo_marcador_zxcv"));
    const auto so_dono = QFileDevice::ReadOwner | QFileDevice::WriteOwner;
    EXPECT_EQ(QFile::permissions(caminho_dados(tmp.path())) & ~(QFileDevice::ReadUser | QFileDevice::WriteUser),
              so_dono);
    EXPECT_EQ(QFile::permissions(caminho_chave(tmp.path())) & ~(QFileDevice::ReadUser | QFileDevice::WriteUser),
              so_dono);
}

TEST(RepositorioFeedbackCifradoTest, InserirMesmaChaveSubstituiVeredito) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    auto repo = repo_em(tmp.path());
    repo->inserir(feedback(QStringLiteral("firefox"), Categoria::PrecisaGpu, QStringLiteral("a")));
    repo->inserir(feedback(QStringLiteral("firefox"), Categoria::CandidatoObvioCpu, QStringLiteral("b")));
    const auto todos = repo->listar_todos();
    ASSERT_EQ(todos.size(), 1);
    EXPECT_EQ(todos.front().veredito, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(todos.front().motivo, QStringLiteral("b"));
}

TEST(RepositorioFeedbackCifradoTest, AtualizarVistoEmERemoverPersistem) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    auto repo = repo_em(tmp.path());
    repo->inserir(feedback(QStringLiteral("firefox"), Categoria::PrecisaGpu, QString{}));
    repo->inserir(feedback(QStringLiteral("slack"), Categoria::CandidatoObvioCpu, QString{}));
    const auto depois = k_t0.addDays(3);
    repo->atualizar_visto_em(1, depois);
    repo->remover(2);

    auto outra = repo_em(tmp.path());
    const auto todos = outra->listar_todos();
    ASSERT_EQ(todos.size(), 1);
    EXPECT_EQ(todos.front().id, 1);
    EXPECT_EQ(todos.front().visto_em, depois);
}

TEST(RepositorioFeedbackCifradoTest, ArquivoAdulteradoNaoCarregaENaoESobrescrito) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    repo_em(tmp.path())->inserir(feedback(QStringLiteral("firefox"), Categoria::PrecisaGpu, QString{}));
    auto bruto = ler_tudo(caminho_dados(tmp.path()));
    bruto[bruto.size() - 1] = static_cast<char>(bruto.at(bruto.size() - 1) ^ 0x01);
    {
        QFile f{caminho_dados(tmp.path())};
        ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(bruto);
    }

    auto repo = repo_em(tmp.path());
    EXPECT_FALSE(repo->buscar(ChaveProcesso::de_binario(QStringLiteral("firefox"))).has_value());
    // Escrita não destrói o arquivo que não conseguimos ler: fica só em memória.
    repo->inserir(feedback(QStringLiteral("slack"), Categoria::CandidatoObvioCpu, QString{}));
    EXPECT_TRUE(repo->buscar(ChaveProcesso::de_binario(QStringLiteral("slack"))).has_value());
    EXPECT_EQ(ler_tudo(caminho_dados(tmp.path())), bruto);
}

TEST(RepositorioFeedbackCifradoTest, DadosSemChaveNaoCarregamENaoSaoSobrescritos) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    repo_em(tmp.path())->inserir(feedback(QStringLiteral("firefox"), Categoria::PrecisaGpu, QString{}));
    const auto original = ler_tudo(caminho_dados(tmp.path()));
    ASSERT_TRUE(QFile::remove(caminho_chave(tmp.path())));

    auto repo = repo_em(tmp.path());
    EXPECT_TRUE(repo->listar_todos().isEmpty());
    repo->inserir(feedback(QStringLiteral("slack"), Categoria::CandidatoObvioCpu, QString{}));
    EXPECT_EQ(ler_tudo(caminho_dados(tmp.path())), original);
    EXPECT_FALSE(QFile::exists(caminho_chave(tmp.path())));
}

TEST(RepositorioFeedbackCifradoTest, ArquivoTruncadoNaoDerruba) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    ASSERT_TRUE(ArquivoChaveLocal{caminho_chave(tmp.path())}.ler_ou_criar().has_value());
    {
        QFile f{caminho_dados(tmp.path())};
        ASSERT_TRUE(f.open(QIODevice::WriteOnly));
        f.write(RepositorioFeedbackCifrado::cabecalho() + QByteArray(5, 'x'));
    }
    EXPECT_TRUE(repo_em(tmp.path())->listar_todos().isEmpty());
}

TEST(RepositorioFeedbackCifradoTest, EntradasDecifradasInvalidasSaoIgnoradas) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    gravar_plaintext_cifrado(tmp.path(), R"({"versao": 1, "proximo_id": 10, "entradas": [
        {"id": 3, "tipo_chave": "binario", "valor_chave": "firefox", "veredito": "PrecisaGpu",
         "motivo": "ok", "criado_em": "2023-11-14T22:13:20Z", "visto_em": "2023-11-14T22:13:20Z"},
        {"id": 4, "tipo_chave": "binario", "valor_chave": "x", "veredito": "Talvez",
         "criado_em": "2023-11-14T22:13:20Z", "visto_em": "2023-11-14T22:13:20Z"},
        {"id": 5, "tipo_chave": "outro", "valor_chave": "y", "veredito": "PrecisaGpu",
         "criado_em": "2023-11-14T22:13:20Z", "visto_em": "2023-11-14T22:13:20Z"},
        {"id": 6, "tipo_chave": "binario", "valor_chave": "", "veredito": "PrecisaGpu",
         "criado_em": "2023-11-14T22:13:20Z", "visto_em": "2023-11-14T22:13:20Z"},
        {"id": -1, "tipo_chave": "binario", "valor_chave": "z", "veredito": "PrecisaGpu",
         "criado_em": "2023-11-14T22:13:20Z", "visto_em": "2023-11-14T22:13:20Z"},
        {"id": 7, "tipo_chave": "binario", "valor_chave": "w", "veredito": "PrecisaGpu",
         "criado_em": "data ruim", "visto_em": "2023-11-14T22:13:20Z"},
        "lixo"
    ]})");
    auto repo = repo_em(tmp.path());
    const auto todos = repo->listar_todos();
    ASSERT_EQ(todos.size(), 1);
    EXPECT_EQ(todos.front().id, 3);
    EXPECT_EQ(todos.front().chave.valor(), QStringLiteral("firefox"));
    repo->inserir(feedback(QStringLiteral("novo"), Categoria::PrecisaGpu, QString{}));
    EXPECT_EQ(repo->buscar(ChaveProcesso::de_binario(QStringLiteral("novo")))->id, 10);
}
