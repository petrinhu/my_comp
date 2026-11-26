#include "infrastructure/security/arquivo_chave_local.h"

#include <QFile>
#include <QFileDevice>
#include <QString>
#include <QTemporaryDir>

#include <gtest/gtest.h>

#include <sys/stat.h>

using my_comp::infrastructure::security::ArquivoChaveLocal;

namespace {

constexpr auto PERMISSOES_0600 = QFileDevice::ReadOwner | QFileDevice::WriteOwner;

bool escrever(const QString& caminho, const QByteArray& dados) {
    QFile f{caminho};
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    return f.write(dados) == dados.size();
}

}

TEST(ArquivoChaveLocalTest, LerSemArquivoDevolveNulloptENaoCria) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString caminho = tmp.path() + QStringLiteral("/k.key");
    const ArquivoChaveLocal chave{caminho};
    EXPECT_FALSE(chave.existe());
    EXPECT_FALSE(chave.ler().has_value());
    EXPECT_FALSE(QFile::exists(caminho));
}

TEST(ArquivoChaveLocalTest, CriaChaveDe32BytesComPermissao0600) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString caminho = tmp.path() + QStringLiteral("/k.key");
    const ArquivoChaveLocal chave{caminho};

    const auto k = chave.ler_ou_criar();
    ASSERT_TRUE(k.has_value());
    EXPECT_EQ(k->size(), ArquivoChaveLocal::TAMANHO_CHAVE);
    EXPECT_TRUE(chave.existe());
    EXPECT_EQ(QFile::permissions(caminho) & ~(QFileDevice::ReadUser | QFileDevice::WriteUser),
              PERMISSOES_0600);
    EXPECT_EQ(QFile{caminho}.size(), 32);
}

TEST(ArquivoChaveLocalTest, SegundaLeituraDevolveAMesmaChave) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString caminho = tmp.path() + QStringLiteral("/k.key");
    const auto primeira = ArquivoChaveLocal{caminho}.ler_ou_criar();
    const auto segunda  = ArquivoChaveLocal{caminho}.ler_ou_criar();
    const auto lida     = ArquivoChaveLocal{caminho}.ler();
    ASSERT_TRUE(primeira && segunda && lida);
    EXPECT_EQ(*primeira, *segunda);
    EXPECT_EQ(*primeira, *lida);
}

TEST(ArquivoChaveLocalTest, ChavesNovasSaoAleatorias) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const auto a = ArquivoChaveLocal{tmp.path() + QStringLiteral("/a.key")}.ler_ou_criar();
    const auto b = ArquivoChaveLocal{tmp.path() + QStringLiteral("/b.key")}.ler_ou_criar();
    ASSERT_TRUE(a && b);
    EXPECT_NE(*a, *b);
    EXPECT_NE(*a, std::vector<std::uint8_t>(32, 0));
}

TEST(ArquivoChaveLocalTest, RecusaArquivoComTamanhoErradoSemSobrescrever) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString caminho = tmp.path() + QStringLiteral("/k.key");
    ASSERT_TRUE(escrever(caminho, QByteArray(31, 'x')));
    ASSERT_TRUE(QFile::setPermissions(caminho, PERMISSOES_0600));

    const ArquivoChaveLocal chave{caminho};
    EXPECT_FALSE(chave.ler().has_value());
    EXPECT_FALSE(chave.ler_ou_criar().has_value());  // não troca a chave de dados já cifrados
    EXPECT_EQ(QFile{caminho}.size(), 31);
}

TEST(ArquivoChaveLocalTest, RecusaChaveLegivelPorGrupoOuOutros) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString caminho = tmp.path() + QStringLiteral("/k.key");
    ASSERT_TRUE(ArquivoChaveLocal{caminho}.ler_ou_criar().has_value());
    ASSERT_EQ(::chmod(caminho.toLocal8Bit().constData(), 0644), 0);
    EXPECT_FALSE(ArquivoChaveLocal{caminho}.ler().has_value());
}

TEST(ArquivoChaveLocalTest, RecusaLinkSimbolico) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString alvo = tmp.path() + QStringLiteral("/alvo.key");
    const QString link = tmp.path() + QStringLiteral("/link.key");
    ASSERT_TRUE(ArquivoChaveLocal{alvo}.ler_ou_criar().has_value());
    ASSERT_TRUE(QFile::link(alvo, link));
    EXPECT_FALSE(ArquivoChaveLocal{link}.ler().has_value());
    EXPECT_FALSE(ArquivoChaveLocal{link}.ler_ou_criar().has_value());
}
