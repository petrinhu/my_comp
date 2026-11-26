#include "presentation/viewmodels/vram_audit_viewmodel.h"

#include <QMetaObject>
#include <QString>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

#include <optional>
#include <utility>

namespace vramopt::presentation::viewmodels {

namespace {

constexpr qsizetype TAMANHO_MAXIMO_CHAVE = 255;

QString chave_binario_de(const domain::Processo& p) {
    for (const auto& c : p.chaves()) {
        if (c.tipo() == domain::TipoChave::Binario) {
            return c.valor();
        }
    }
    return {};
}

std::optional<domain::Categoria> categoria_de_feedback(const QString& texto) {
    if (texto == QStringLiteral("PrecisaGpu")) {
        return domain::Categoria::PrecisaGpu;
    }
    if (texto == QStringLiteral("CandidatoObvioCpu")) {
        return domain::Categoria::CandidatoObvioCpu;
    }
    return std::nullopt;
}

QString fabricanteString(domain::Fabricante f) {
    switch (f) {
        case domain::Fabricante::Nvidia:       return QStringLiteral("Nvidia");
        case domain::Fabricante::Intel:        return QStringLiteral("Intel");
        case domain::Fabricante::Amd:          return QStringLiteral("AMD");
        case domain::Fabricante::Desconhecido: return QStringLiteral("Desconhecido");
    }
    return QStringLiteral("?");
}

QString categoriaString(domain::Categoria c) {
    switch (c) {
        case domain::Categoria::CandidatoObvioCpu: return QStringLiteral("CandidatoObvioCpu");
        case domain::Categoria::Indeterminado:     return QStringLiteral("Indeterminado");
        case domain::Categoria::PrecisaGpu:        return QStringLiteral("PrecisaGpu");
    }
    return QStringLiteral("?");
}

QString sugestaoTipoString(application::TipoSugestao t) {
    switch (t) {
        case application::TipoSugestao::Nenhuma: return QStringLiteral("Nenhuma");
        case application::TipoSugestao::AjustarConfigDoApp:
            return QStringLiteral("AjustarConfigDoApp");
        case application::TipoSugestao::AvaliacaoManualNecessaria:
            return QStringLiteral("AvaliacaoManualNecessaria");
    }
    return QStringLiteral("?");
}

QString categoriaOrigemString(domain::CategoriaOrigem c) {
    switch (c) {
        case domain::CategoriaOrigem::BrowserChromium: return QStringLiteral("BrowserChromium");
        case domain::CategoriaOrigem::Firefox:         return QStringLiteral("Firefox");
        case domain::CategoriaOrigem::PrimeEnv:        return QStringLiteral("PrimeEnv");
        case domain::CategoriaOrigem::DesktopWrapper:  return QStringLiteral("DesktopWrapper");
    }
    return QStringLiteral("?");
}

QString confiancaString(domain::Confianca c) {
    switch (c) {
        case domain::Confianca::Baixa: return QStringLiteral("Baixa");
        case domain::Confianca::Media: return QStringLiteral("Media");
        case domain::Confianca::Alta:  return QStringLiteral("Alta");
    }
    return QStringLiteral("?");
}

QVariantMap origemToMap(const domain::OrigemConfig& o) {
    QVariantMap m;
    m.insert(QStringLiteral("categoria"),       categoriaOrigemString(o.categoria));
    m.insert(QStringLiteral("tipo"),            o.tipo);
    m.insert(QStringLiteral("caminho"),         o.caminho);
    m.insert(QStringLiteral("chave"),           o.chave);
    m.insert(QStringLiteral("valorAtual"),      o.valor_atual);
    m.insert(QStringLiteral("valorSugerido"),   o.valor_sugerido);
    m.insert(QStringLiteral("instrucao"),       o.instrucao_humana);
    m.insert(QStringLiteral("confianca"),       confiancaString(o.confianca));
    m.insert(QStringLiteral("requerReinicio"),  o.requer_reinicio_app);
    m.insert(QStringLiteral("aplicavelAuto"),   o.aplicavel_automatico);
    return m;
}

QVariantMap itemToMap(const application::ItemAuditoriaVram& item) {
    QVariantMap m;
    m.insert(QStringLiteral("pid"),            item.processo.pid().valor());
    m.insert(QStringLiteral("nome"),           item.processo.nome().valor());
    m.insert(QStringLiteral("chaveBinario"),   chave_binario_de(item.processo));
    m.insert(QStringLiteral("vramBytes"),
             static_cast<qulonglong>(item.processo.vramConsumida().valor()));
    m.insert(QStringLiteral("vramMb"),
             static_cast<double>(item.processo.vramConsumida().valor()) / (1024.0 * 1024.0));
    m.insert(QStringLiteral("categoria"),      categoriaString(item.classificacao.categoria));
    m.insert(QStringLiteral("regra"),          item.classificacao.justificativa.regra);
    m.insert(QStringLiteral("detalhe"),        item.classificacao.justificativa.detalhe);
    m.insert(QStringLiteral("sugestaoTipo"),   sugestaoTipoString(item.sugestao.tipo));
    m.insert(QStringLiteral("sugestaoDescricao"), item.sugestao.descricao);

    QVariantList origens;
    origens.reserve(item.origens_config.size());
    for (const auto& o : item.origens_config) {
        origens.append(origemToMap(o));
    }
    m.insert(QStringLiteral("origensConfig"), origens);
    return m;
}

QVariantMap grupoToMap(const application::GrupoAuditoriaGpu& grupo) {
    QVariantMap m;
    m.insert(QStringLiteral("gpuId"),         grupo.gpu.id());
    m.insert(QStringLiteral("gpuFabricante"), fabricanteString(grupo.gpu.fabricante()));
    m.insert(QStringLiteral("gpuModelo"),     grupo.gpu.modelo());
    m.insert(QStringLiteral("gpuVramTotalBytes"),
             static_cast<qulonglong>(grupo.gpu.vramTotal().valor()));
    m.insert(QStringLiteral("gpuVramTotalMb"),
             static_cast<double>(grupo.gpu.vramTotal().valor()) / (1024.0 * 1024.0));

    QVariantList itens;
    itens.reserve(static_cast<int>(grupo.itens.size()));
    for (const auto& item : grupo.itens) {
        itens.append(itemToMap(item));
    }
    m.insert(QStringLiteral("itens"), itens);
    return m;
}

QVariantList relatorioToVariant(const application::RelatorioAuditoriaVram& r) {
    QVariantList out;
    out.reserve(static_cast<int>(r.grupos.size()));
    for (const auto& g : r.grupos) {
        out.append(grupoToMap(g));
    }
    return out;
}

}

VramAuditViewModel::VramAuditViewModel(
    std::shared_ptr<application::ServicoAuditarVramAtual> servico,
    std::shared_ptr<application::ServicoRegistrarFeedback> feedback,
    QObject* parent)
    : QObject{parent}, m_servico{std::move(servico)}, m_feedback{std::move(feedback)} {}

void VramAuditViewModel::aplicarRelatorio(
    const application::RelatorioAuditoriaVram& relatorio) {
    m_grupos = relatorioToVariant(relatorio);
    emit gruposChanged();
}

void VramAuditViewModel::refresh() {
    if (!m_servico) {
        return;
    }
    bool esperado = false;
    if (!m_busy.compare_exchange_strong(esperado, true)) {
        return;
    }
    emit isBusyChanged();

    auto servico = m_servico;
    auto _future = QtConcurrent::run([this, servico]() {
        const auto relatorio = servico->executar();
        QMetaObject::invokeMethod(this, [this, relatorio]() {
            m_grupos = relatorioToVariant(relatorio);
            emit gruposChanged();
            m_busy.store(false);
            emit isBusyChanged();
        }, Qt::QueuedConnection);
    });
}

void VramAuditViewModel::registrarFeedback(const QString& chaveBinario,
                                           const QString& categoria,
                                           const QString& motivo) {
    const auto veredito = categoria_de_feedback(categoria);
    const auto binario = chaveBinario.trimmed();
    if (!m_feedback || !veredito || binario.isEmpty() || binario.size() > TAMANHO_MAXIMO_CHAVE) {
        emit feedbackRegistrado(false);
        return;
    }
    auto servico = m_feedback;
    auto chave = domain::ChaveProcesso::deBinario(binario);
    auto _future = QtConcurrent::run([this, servico, chave, v = *veredito, motivo]() {
        const bool ok = servico->executar(chave, v, motivo);
        QMetaObject::invokeMethod(this, [this, ok]() {
            emit feedbackRegistrado(ok);
            if (ok) {
                refresh();
            }
        }, Qt::QueuedConnection);
    });
}

}
