#include "infrastructure/collectors/vram/catalogo_classificacao_json.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QSet>
#include <QStringList>
#include <QtGlobal>

#include <optional>
#include <utility>

// Q_INIT_RESOURCE precisa ficar fora de namespace. A my_comp_core é estática: sem esta
// referência o linker descartaria o objeto do .qrc e o recurso não existiria no binário.
static void inicializar_recurso_catalogo_vram() {
    Q_INIT_RESOURCE(catalogo_vram);
}

namespace my_comp::infrastructure::vram {

namespace {

constexpr int VERSAO_SUPORTADA = 1;
constexpr qsizetype TAMANHO_MAXIMO_JSON = qsizetype{1024} * 1024;  // o catálogo real tem poucos KiB
constexpr qsizetype TAMANHO_MAXIMO_CAMPO = 512;
constexpr qsizetype MAXIMO_CHAVES_POR_LISTA = 64;
constexpr QLatin1String CAMINHO_RECURSO{":/vram/catalogo.json"};

std::optional<domain::vram::Categoria> categoria_de_texto(const QString& texto) {
    if (texto == QStringLiteral("CandidatoObvioCpu")) {
        return domain::vram::Categoria::CandidatoObvioCpu;
    }
    if (texto == QStringLiteral("PrecisaGpu")) {
        return domain::vram::Categoria::PrecisaGpu;
    }
    return std::nullopt;  // "Indeterminado" incluso: ver cabeçalho
}

std::optional<QString> texto_obrigatorio(const QJsonObject& obj, const QString& campo) {
    const auto v = obj.value(campo);
    if (!v.isString()) {
        return std::nullopt;
    }
    auto s = v.toString().trimmed();
    if (s.isEmpty() || s.size() > TAMANHO_MAXIMO_CAMPO) {
        return std::nullopt;
    }
    return s;
}

// Campo ausente = lista vazia; presente precisa ser lista de strings não vazias.
std::optional<QStringList> lista_de_chaves(const QJsonObject& obj, const QString& campo) {
    if (!obj.contains(campo)) {
        return QStringList{};
    }
    const auto v = obj.value(campo);
    if (!v.isArray()) {
        return std::nullopt;
    }
    const auto arr = v.toArray();
    if (arr.size() > MAXIMO_CHAVES_POR_LISTA) {
        return std::nullopt;
    }
    QStringList out;
    for (const auto& item : arr) {
        if (!item.isString()) {
            return std::nullopt;
        }
        const auto s = item.toString().trimmed();
        if (s.isEmpty() || s.size() > TAMANHO_MAXIMO_CAMPO) {
            return std::nullopt;
        }
        out.append(s);
    }
    return out;
}

std::optional<domain::vram::EntradaCatalogo> validar_entrada(const QJsonValue& valor, QString& motivo) {
    if (!valor.isObject()) {
        motivo = QStringLiteral("não é objeto");
        return std::nullopt;
    }
    const auto obj = valor.toObject();
    const auto id = texto_obrigatorio(obj, QStringLiteral("id"));
    if (!id) {
        motivo = QStringLiteral("id ausente ou inválido");
        return std::nullopt;
    }
    const auto veredito_txt = texto_obrigatorio(obj, QStringLiteral("veredito"));
    const auto veredito = veredito_txt ? categoria_de_texto(*veredito_txt) : std::nullopt;
    if (!veredito) {
        motivo = QStringLiteral("veredito ausente ou fora de {CandidatoObvioCpu, PrecisaGpu}");
        return std::nullopt;
    }
    const auto justificativa = texto_obrigatorio(obj, QStringLiteral("justificativa"));
    if (!justificativa) {
        motivo = QStringLiteral("justificativa ausente");
        return std::nullopt;
    }
    const auto binarios   = lista_de_chaves(obj, QStringLiteral("binarios"));
    const auto wm_class   = lista_de_chaves(obj, QStringLiteral("wm_class"));
    const auto desktop_id = lista_de_chaves(obj, QStringLiteral("desktop_id"));
    if (!binarios || !wm_class || !desktop_id) {
        motivo = QStringLiteral("lista de chaves malformada");
        return std::nullopt;
    }
    if (binarios->isEmpty() && wm_class->isEmpty() && desktop_id->isEmpty()) {
        motivo = QStringLiteral("nenhuma chave (binarios/wm_class/desktop_id)");
        return std::nullopt;
    }
    return domain::vram::EntradaCatalogo{
        .id            = *id,
        .binarios      = *binarios,
        .wm_class      = *wm_class,
        .desktop_id    = *desktop_id,
        .veredito      = *veredito,
        .justificativa = *justificativa,
    };
}

void indexar(QHash<QString, std::size_t>& indice, const QStringList& chaves, std::size_t pos,
             const QString& id, bool ignorar_caixa) {
    for (const auto& c : chaves) {
        const auto chave = ignorar_caixa ? c.toLower() : c;
        if (indice.contains(chave)) {
            qWarning("catalogo VRAM: chave '%s' de '%s' já pertence a outra entrada; mantida a primeira",
                     qUtf8Printable(c), qUtf8Printable(id));
            continue;
        }
        indice.insert(chave, pos);
    }
}

}  // namespace

ResultadoCargaCatalogo parsear_catalogo_json(const QByteArray& json) {
    ResultadoCargaCatalogo resultado;
    if (json.isEmpty() || json.size() > TAMANHO_MAXIMO_JSON) {
        qWarning("catalogo VRAM: JSON vazio ou acima de %lld bytes; catálogo ignorado",
                 static_cast<long long>(TAMANHO_MAXIMO_JSON));
        return resultado;
    }
    QJsonParseError erro{};
    const auto doc = QJsonDocument::fromJson(json, &erro);
    if (erro.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning("catalogo VRAM: JSON inválido (%s); catálogo ignorado",
                 qUtf8Printable(erro.errorString()));
        return resultado;
    }
    const auto raiz = doc.object();
    const auto versao = raiz.value(QStringLiteral("versao"));
    if (!versao.isDouble() || versao.toInt(-1) != VERSAO_SUPORTADA) {
        qWarning("catalogo VRAM: versão ausente ou não suportada; catálogo ignorado");
        return resultado;
    }
    const auto entradas = raiz.value(QStringLiteral("entradas"));
    if (!entradas.isArray()) {
        qWarning("catalogo VRAM: campo 'entradas' ausente; catálogo ignorado");
        return resultado;
    }

    QSet<QString> ids;
    const auto arr = entradas.toArray();
    for (qsizetype i = 0; i < arr.size(); ++i) {
        QString motivo;
        auto entrada = validar_entrada(arr.at(i), motivo);
        if (entrada && ids.contains(entrada->id)) {
            motivo = QStringLiteral("id '%1' repetido").arg(entrada->id);
            entrada.reset();
        }
        if (!entrada) {
            qWarning("catalogo VRAM: entrada %lld ignorada: %s",
                     static_cast<long long>(i), qUtf8Printable(motivo));
            ++resultado.rejeitadas;
            continue;
        }
        ids.insert(entrada->id);
        resultado.entradas.push_back(std::move(*entrada));
    }
    return resultado;
}

ResultadoCargaCatalogo carregar_catalogo_empacotado() {
    inicializar_recurso_catalogo_vram();
    QFile f{QString{CAMINHO_RECURSO}};
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("catalogo VRAM: recurso %s ausente; catálogo vazio", CAMINHO_RECURSO.data());
        return {};
    }
    return parsear_catalogo_json(f.read(TAMANHO_MAXIMO_JSON + 1));
}

CatalogoClassificacaoJson::CatalogoClassificacaoJson(std::vector<domain::vram::EntradaCatalogo> entradas)
    : m_entradas{std::move(entradas)} {
    for (std::size_t pos = 0; pos < m_entradas.size(); ++pos) {
        const auto& e = m_entradas[pos];
        indexar(m_por_binario,   e.binarios,   pos, e.id, false);  // comm diferencia caixa
        indexar(m_por_wm_class,  e.wm_class,   pos, e.id, true);
        indexar(m_por_desktop_id, e.desktop_id, pos, e.id, true);
    }
}

std::optional<domain::vram::EntradaCatalogo>
CatalogoClassificacaoJson::buscar(const domain::vram::ChaveProcesso& chave) const {
    const QHash<QString, std::size_t>* indice = nullptr;
    QString valor = chave.valor();
    switch (chave.tipo()) {
        case domain::vram::TipoChave::Binario:
            indice = &m_por_binario;
            break;
        case domain::vram::TipoChave::WmClass:
            indice = &m_por_wm_class;
            valor = valor.toLower();
            break;
        case domain::vram::TipoChave::DesktopId:
            indice = &m_por_desktop_id;
            valor = valor.toLower();
            break;
    }
    if (indice == nullptr) {
        return std::nullopt;
    }
    const auto it = indice->constFind(valor);
    if (it == indice->cend()) {
        return std::nullopt;
    }
    return m_entradas[it.value()];
}

}
