#include "infrastructure/collectors/vram/repositorio_feedback_cifrado.h"

#include "infrastructure/security/arquivo_chave_local.h"

#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QSaveFile>
#include <QSet>
#include <QtGlobal>

#include <openssl/crypto.h>

#include <algorithm>
#include <exception>
#include <utility>
#include <vector>

namespace vramopt::infrastructure {

namespace {

using my_comp::infrastructure::security::ArquivoChaveLocal;

constexpr int       VERSAO_PLAINTEXT        = 1;
constexpr qsizetype TAMANHO_IV              = 12;
constexpr qsizetype TAMANHO_TAG             = 16;
constexpr qint64    TAMANHO_MAXIMO_ARQUIVO  = 4LL * 1024 * 1024;
constexpr qsizetype TAMANHO_MAXIMO_VALOR    = 512;
constexpr qsizetype TAMANHO_MAXIMO_MOTIVO   = 2048;
constexpr auto      PERMISSOES_SO_DONO      = QFileDevice::ReadOwner | QFileDevice::WriteOwner;
constexpr auto      PERMISSOES_DIR_SO_DONO  =
    QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner;

const char* const NOME_DADOS = "vram_feedback.bin";
const char* const NOME_CHAVE = "vram_feedback.key";

void limpar(std::vector<std::uint8_t>& v) { OPENSSL_cleanse(v.data(), v.size()); }
void limpar(QByteArray& b) { OPENSSL_cleanse(b.data(), static_cast<std::size_t>(b.size())); }

QString texto_do_tipo(domain::TipoChave t) {
    switch (t) {
        case domain::TipoChave::Binario:   return QStringLiteral("binario");
        case domain::TipoChave::WmClass:   return QStringLiteral("wm_class");
        case domain::TipoChave::DesktopId: return QStringLiteral("desktop_id");
    }
    return QStringLiteral("?");
}

std::optional<domain::ChaveProcesso> chave_de(const QString& tipo, const QString& valor) {
    if (valor.isEmpty() || valor.size() > TAMANHO_MAXIMO_VALOR) {
        return std::nullopt;
    }
    if (tipo == QStringLiteral("binario"))    return domain::ChaveProcesso::de_binario(valor);
    if (tipo == QStringLiteral("wm_class"))   return domain::ChaveProcesso::de_wm_class(valor);
    if (tipo == QStringLiteral("desktop_id")) return domain::ChaveProcesso::de_desktop_id(valor);
    return std::nullopt;
}

QString texto_da_categoria(domain::Categoria c) {
    switch (c) {
        case domain::Categoria::CandidatoObvioCpu: return QStringLiteral("CandidatoObvioCpu");
        case domain::Categoria::Indeterminado:     return QStringLiteral("Indeterminado");
        case domain::Categoria::PrecisaGpu:        return QStringLiteral("PrecisaGpu");
    }
    return QStringLiteral("?");
}

std::optional<domain::Categoria> categoria_de(const QString& texto) {
    if (texto == QStringLiteral("CandidatoObvioCpu")) return domain::Categoria::CandidatoObvioCpu;
    if (texto == QStringLiteral("PrecisaGpu"))        return domain::Categoria::PrecisaGpu;
    if (texto == QStringLiteral("Indeterminado"))     return domain::Categoria::Indeterminado;
    return std::nullopt;
}

QString texto_da_data(const QDateTime& d) { return d.toUTC().toString(Qt::ISODateWithMs); }

std::optional<QDateTime> data_de(const QJsonValue& v) {
    if (!v.isString()) {
        return std::nullopt;
    }
    auto d = QDateTime::fromString(v.toString(), Qt::ISODateWithMs);
    if (!d.isValid()) {
        return std::nullopt;
    }
    return d;
}

QJsonObject para_json(const domain::EntradaFeedback& e) {
    QJsonObject o{
        {QStringLiteral("id"),          e.id},
        {QStringLiteral("tipo_chave"),  texto_do_tipo(e.chave.tipo())},
        {QStringLiteral("valor_chave"), e.chave.valor()},
        {QStringLiteral("veredito"),    texto_da_categoria(e.veredito)},
        {QStringLiteral("motivo"),      e.motivo},
        {QStringLiteral("criado_em"),   texto_da_data(e.criado_em)},
        {QStringLiteral("visto_em"),    texto_da_data(e.visto_em)},
    };
    if (e.revisado_em.has_value()) {
        o.insert(QStringLiteral("revisado_em"), texto_da_data(*e.revisado_em));
    }
    return o;
}

std::optional<domain::EntradaFeedback> de_json(const QJsonValue& v) {
    if (!v.isObject()) {
        return std::nullopt;
    }
    const auto o = v.toObject();
    const int id = o.value(QStringLiteral("id")).toInt(0);
    const auto tipo  = o.value(QStringLiteral("tipo_chave"));
    const auto valor = o.value(QStringLiteral("valor_chave"));
    const auto veredito = o.value(QStringLiteral("veredito"));
    const auto motivo = o.value(QStringLiteral("motivo"));
    if (id <= 0 || !tipo.isString() || !valor.isString() || !veredito.isString()
        || (!motivo.isUndefined() && !motivo.isString())) {
        return std::nullopt;
    }
    auto chave = chave_de(tipo.toString(), valor.toString());
    auto categoria = categoria_de(veredito.toString());
    auto criado = data_de(o.value(QStringLiteral("criado_em")));
    auto visto  = data_de(o.value(QStringLiteral("visto_em")));
    if (!chave || !categoria || !criado || !visto || motivo.toString().size() > TAMANHO_MAXIMO_MOTIVO) {
        return std::nullopt;
    }
    std::optional<QDateTime> revisado;
    if (o.contains(QStringLiteral("revisado_em"))) {
        revisado = data_de(o.value(QStringLiteral("revisado_em")));
        if (!revisado) {
            return std::nullopt;
        }
    }
    return domain::EntradaFeedback{
        .id = id,
        .chave = std::move(*chave),
        .veredito = *categoria,
        .motivo = motivo.toString(),
        .criado_em = *criado,
        .visto_em = *visto,
        .revisado_em = revisado,
    };
}

QByteArray bytes_de(const std::vector<std::uint8_t>& v) {
    return QByteArray{reinterpret_cast<const char*>(v.data()), static_cast<qsizetype>(v.size())};
}

std::vector<std::uint8_t> vetor_de(QByteArrayView b) {
    const auto* p = reinterpret_cast<const std::uint8_t*>(b.data());
    return std::vector<std::uint8_t>(p, p + b.size());
}

}  // namespace

RepositorioFeedbackCifrado::RepositorioFeedbackCifrado(
    std::shared_ptr<my_comp::domain::interfaces::ISecurityVault> vault, QString diretorio)
    : m_vault{std::move(vault)}, m_diretorio{std::move(diretorio)} {}

QByteArray RepositorioFeedbackCifrado::cabecalho() {
    return QByteArrayLiteral("MCFB\x01");
}

QString RepositorioFeedbackCifrado::caminho_dados() const {
    return m_diretorio + QLatin1Char('/') + QLatin1String(NOME_DADOS);
}

QString RepositorioFeedbackCifrado::caminho_chave() const {
    return m_diretorio + QLatin1Char('/') + QLatin1String(NOME_CHAVE);
}

void RepositorioFeedbackCifrado::garantir_carregado() const {
    if (!m_carregado) {
        m_carregado = true;
        carregar();
    }
}

void RepositorioFeedbackCifrado::carregar() const {
    QFile f{caminho_dados()};
    if (!f.exists()) {
        return;  // primeiro uso: nada a ler, nenhuma chave criada
    }
    // Daqui em diante existe algo em disco; qualquer falha deixa o arquivo intocado.
    m_somente_memoria = true;
    if (!m_vault) {
        qWarning("feedback VRAM: sem vault; feedback só em memória");
        return;
    }
    if (f.size() > TAMANHO_MAXIMO_ARQUIVO || !f.open(QIODevice::ReadOnly)) {
        qWarning("feedback VRAM: %s ilegível ou grande demais; feedback só em memória",
                 qUtf8Printable(caminho_dados()));
        return;
    }
    const QByteArray bruto = f.read(TAMANHO_MAXIMO_ARQUIVO);
    const QByteArray cab = cabecalho();
    if (!bruto.startsWith(cab) || bruto.size() < cab.size() + TAMANHO_IV + TAMANHO_TAG) {
        qWarning("feedback VRAM: %s com formato desconhecido; feedback só em memória",
                 qUtf8Printable(caminho_dados()));
        return;
    }
    auto chave = ArquivoChaveLocal{caminho_chave()}.ler();
    if (!chave) {
        qWarning("feedback VRAM: chave ausente ou recusada; feedback só em memória");
        return;
    }
    const QByteArrayView resto = QByteArrayView{bruto}.sliced(cab.size());
    my_comp::domain::models::VaultData dados{
        .ciphertext = vetor_de(resto.sliced(TAMANHO_IV + TAMANHO_TAG)),
        .iv         = vetor_de(resto.first(TAMANHO_IV)),
        .tag        = vetor_de(resto.sliced(TAMANHO_IV, TAMANHO_TAG)),
    };
    auto plaintext = m_vault->decrypt(dados, *chave);
    limpar(*chave);
    if (plaintext.empty()) {
        qWarning("feedback VRAM: falha de integridade ao decifrar; feedback só em memória");
        return;
    }
    QByteArray json = bytes_de(plaintext);
    limpar(plaintext);
    QJsonParseError erro{};
    const auto doc = QJsonDocument::fromJson(json, &erro);
    limpar(json);
    if (erro.error != QJsonParseError::NoError || !doc.isObject()
        || doc.object().value(QStringLiteral("versao")).toInt(-1) != VERSAO_PLAINTEXT) {
        qWarning("feedback VRAM: conteúdo decifrado inválido; feedback só em memória");
        return;
    }

    const auto raiz = doc.object();
    int maior_id = 0;
    QSet<int> ids;
    for (const auto& v : raiz.value(QStringLiteral("entradas")).toArray()) {
        auto e = de_json(v);
        const bool chave_repetida = e && std::any_of(m_entradas.cbegin(), m_entradas.cend(),
            [&e](const domain::EntradaFeedback& x) { return x.chave == e->chave; });
        if (!e || ids.contains(e->id) || chave_repetida) {
            qWarning("feedback VRAM: entrada inválida ignorada");
            continue;
        }
        ids.insert(e->id);
        maior_id = std::max(maior_id, e->id);
        m_entradas.push_back(std::move(*e));
    }
    m_proximo_id = std::max(maior_id + 1, raiz.value(QStringLiteral("proximo_id")).toInt(1));
    m_somente_memoria = false;
}

void RepositorioFeedbackCifrado::persistir() {
    if (m_somente_memoria || !m_vault) {
        qWarning("feedback VRAM: arquivo existente não pôde ser lido; alteração mantida só em memória");
        return;
    }
    if (!QDir{}.mkpath(m_diretorio)) {
        qWarning("feedback VRAM: não foi possível criar %s", qUtf8Printable(m_diretorio));
        return;
    }
    QFile::setPermissions(m_diretorio, PERMISSOES_DIR_SO_DONO);

    auto chave = ArquivoChaveLocal{caminho_chave()}.ler_ou_criar();
    if (!chave) {
        qWarning("feedback VRAM: sem chave utilizável; alteração mantida só em memória");
        return;
    }

    QJsonArray entradas;
    for (const auto& e : std::as_const(m_entradas)) {
        entradas.append(para_json(e));
    }
    QByteArray json = QJsonDocument{QJsonObject{
        {QStringLiteral("versao"),     VERSAO_PLAINTEXT},
        {QStringLiteral("proximo_id"), m_proximo_id},
        {QStringLiteral("entradas"),   entradas},
    }}.toJson(QJsonDocument::Compact);
    auto plaintext = vetor_de(json);
    limpar(json);

    my_comp::domain::models::VaultData dados;
    try {
        dados = m_vault->encrypt(plaintext, *chave);
    } catch (const std::exception& ex) {
        qWarning("feedback VRAM: falha ao cifrar (%s)", ex.what());
    }
    limpar(plaintext);
    limpar(*chave);
    if (dados.iv.size() != static_cast<std::size_t>(TAMANHO_IV)
        || dados.tag.size() != static_cast<std::size_t>(TAMANHO_TAG)) {
        return;
    }

    QSaveFile f{caminho_dados()};  // grava em temporário e renomeia: nunca deixa arquivo pela metade
    if (!f.open(QIODevice::WriteOnly) || !f.setPermissions(PERMISSOES_SO_DONO)) {
        qWarning("feedback VRAM: não foi possível gravar %s", qUtf8Printable(caminho_dados()));
        return;
    }
    f.write(cabecalho());
    f.write(bytes_de(dados.iv));
    f.write(bytes_de(dados.tag));
    f.write(bytes_de(dados.ciphertext));
    if (!f.commit()) {
        qWarning("feedback VRAM: falha ao confirmar gravação de %s", qUtf8Printable(caminho_dados()));
    }
}

std::optional<domain::EntradaFeedback>
RepositorioFeedbackCifrado::buscar(const domain::ChaveProcesso& chave) const {
    const std::lock_guard trava{m_mutex};
    garantir_carregado();
    for (const auto& e : std::as_const(m_entradas)) {
        if (e.chave == chave) {
            return e;
        }
    }
    return std::nullopt;
}

void RepositorioFeedbackCifrado::inserir(const domain::EntradaFeedback& entrada) {
    const std::lock_guard trava{m_mutex};
    garantir_carregado();
    domain::EntradaFeedback nova = entrada;
    if (nova.id <= 0) {
        nova.id = m_proximo_id;
    }
    m_proximo_id = std::max(m_proximo_id, nova.id + 1);
    m_entradas.removeIf([&nova](const domain::EntradaFeedback& e) {
        return e.id == nova.id || e.chave == nova.chave;
    });
    m_entradas.push_back(std::move(nova));
    persistir();
}

void RepositorioFeedbackCifrado::atualizar_visto_em(int id, QDateTime quando) {
    const std::lock_guard trava{m_mutex};
    garantir_carregado();
    for (auto& e : m_entradas) {
        if (e.id == id) {
            e.visto_em = std::move(quando);
            persistir();
            return;
        }
    }
}

void RepositorioFeedbackCifrado::remover(int id) {
    const std::lock_guard trava{m_mutex};
    garantir_carregado();
    if (m_entradas.removeIf([id](const domain::EntradaFeedback& e) { return e.id == id; }) > 0) {
        persistir();
    }
}

QVector<domain::EntradaFeedback> RepositorioFeedbackCifrado::listar_todos() const {
    const std::lock_guard trava{m_mutex};
    garantir_carregado();
    return m_entradas;
}

}
