#include "presentation/viewmodels/vram_audit_viewmodel.h"

#include <QMetaObject>
#include <QString>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

#include <optional>
#include <utility>

namespace my_comp::presentation::viewmodels {

namespace {

constexpr qsizetype TAMANHO_MAXIMO_CHAVE = 255;

QString chave_binario_de(const domain::vram::Processo& p) {
    for (const auto& c : p.chaves()) {
        if (c.tipo() == domain::vram::TipoChave::Binario) {
            return c.valor();
        }
    }
    return {};
}

std::optional<domain::vram::Categoria> categoria_de_feedback(const QString& texto) {
    if (texto == QStringLiteral("PrecisaGpu")) {
        return domain::vram::Categoria::PrecisaGpu;
    }
    if (texto == QStringLiteral("CandidatoObvioCpu")) {
        return domain::vram::Categoria::CandidatoObvioCpu;
    }
    return std::nullopt;
}

QString fabricante_string(domain::vram::Fabricante f) {
    switch (f) {
        case domain::vram::Fabricante::Nvidia:       return QStringLiteral("Nvidia");
        case domain::vram::Fabricante::Intel:        return QStringLiteral("Intel");
        case domain::vram::Fabricante::Amd:          return QStringLiteral("AMD");
        case domain::vram::Fabricante::Desconhecido: return QStringLiteral("Desconhecido");
    }
    return QStringLiteral("?");
}

QString categoria_string(domain::vram::Categoria c) {
    switch (c) {
        case domain::vram::Categoria::CandidatoObvioCpu: return QStringLiteral("CandidatoObvioCpu");
        case domain::vram::Categoria::Indeterminado:     return QStringLiteral("Indeterminado");
        case domain::vram::Categoria::PrecisaGpu:        return QStringLiteral("PrecisaGpu");
    }
    return QStringLiteral("?");
}

QString sugestao_tipo_string(application::vram::TipoSugestao t) {
    switch (t) {
        case application::vram::TipoSugestao::Nenhuma: return QStringLiteral("Nenhuma");
        case application::vram::TipoSugestao::AjustarConfigDoApp:
            return QStringLiteral("AjustarConfigDoApp");
        case application::vram::TipoSugestao::AvaliacaoManualNecessaria:
            return QStringLiteral("AvaliacaoManualNecessaria");
    }
    return QStringLiteral("?");
}

QString categoria_origem_string(domain::vram::CategoriaOrigem c) {
    switch (c) {
        case domain::vram::CategoriaOrigem::BrowserChromium: return QStringLiteral("BrowserChromium");
        case domain::vram::CategoriaOrigem::Firefox:         return QStringLiteral("Firefox");
        case domain::vram::CategoriaOrigem::PrimeEnv:        return QStringLiteral("PrimeEnv");
        case domain::vram::CategoriaOrigem::DesktopWrapper:  return QStringLiteral("DesktopWrapper");
    }
    return QStringLiteral("?");
}

QString confianca_string(domain::vram::Confianca c) {
    switch (c) {
        case domain::vram::Confianca::Baixa: return QStringLiteral("Baixa");
        case domain::vram::Confianca::Media: return QStringLiteral("Media");
        case domain::vram::Confianca::Alta:  return QStringLiteral("Alta");
    }
    return QStringLiteral("?");
}

QVariantMap origem_to_map(const domain::vram::OrigemConfig& o) {
    QVariantMap m;
    m.insert(QStringLiteral("categoria"),       categoria_origem_string(o.categoria));
    m.insert(QStringLiteral("tipo"),            o.tipo);
    m.insert(QStringLiteral("caminho"),         o.caminho);
    m.insert(QStringLiteral("chave"),           o.chave);
    m.insert(QStringLiteral("valorAtual"),      o.valor_atual);
    m.insert(QStringLiteral("valorSugerido"),   o.valor_sugerido);
    m.insert(QStringLiteral("instrucao"),       o.instrucao_humana);
    m.insert(QStringLiteral("confianca"),       confianca_string(o.confianca));
    m.insert(QStringLiteral("requerReinicio"),  o.requer_reinicio_app);
    m.insert(QStringLiteral("aplicavelAuto"),   o.aplicavel_automatico);
    return m;
}

QVariantMap item_to_map(const application::vram::ItemAuditoriaVram& item) {
    QVariantMap m;
    m.insert(QStringLiteral("pid"),            item.processo.pid().valor());
    m.insert(QStringLiteral("nome"),           item.processo.nome().valor());
    m.insert(QStringLiteral("chaveBinario"),   chave_binario_de(item.processo));
    m.insert(QStringLiteral("vramBytes"),
             static_cast<qulonglong>(item.processo.vram_consumida().valor()));
    m.insert(QStringLiteral("vramMb"),
             static_cast<double>(item.processo.vram_consumida().valor()) / (1024.0 * 1024.0));
    m.insert(QStringLiteral("categoria"),      categoria_string(item.classificacao.categoria));
    m.insert(QStringLiteral("regra"),          item.classificacao.justificativa.regra);
    m.insert(QStringLiteral("detalhe"),        item.classificacao.justificativa.detalhe);
    m.insert(QStringLiteral("sugestaoTipo"),   sugestao_tipo_string(item.sugestao.tipo));
    m.insert(QStringLiteral("sugestaoDescricao"), item.sugestao.descricao);

    QVariantList origens;
    origens.reserve(item.origens_config.size());
    for (const auto& o : item.origens_config) {
        origens.append(origem_to_map(o));
    }
    m.insert(QStringLiteral("origensConfig"), origens);
    return m;
}

QVariantMap grupo_to_map(const application::vram::GrupoAuditoriaGpu& grupo) {
    QVariantMap m;
    m.insert(QStringLiteral("gpuId"),         grupo.gpu.id());
    m.insert(QStringLiteral("gpuFabricante"), fabricante_string(grupo.gpu.fabricante()));
    m.insert(QStringLiteral("gpuModelo"),     grupo.gpu.modelo());
    m.insert(QStringLiteral("gpuVramTotalBytes"),
             static_cast<qulonglong>(grupo.gpu.vram_total().valor()));
    m.insert(QStringLiteral("gpuVramTotalMb"),
             static_cast<double>(grupo.gpu.vram_total().valor()) / (1024.0 * 1024.0));

    QVariantList itens;
    itens.reserve(static_cast<int>(grupo.itens.size()));
    for (const auto& item : grupo.itens) {
        itens.append(item_to_map(item));
    }
    m.insert(QStringLiteral("itens"), itens);
    return m;
}

QVariantList relatorio_to_variant(const application::vram::RelatorioAuditoriaVram& r) {
    QVariantList out;
    out.reserve(static_cast<int>(r.grupos.size()));
    for (const auto& g : r.grupos) {
        out.append(grupo_to_map(g));
    }
    return out;
}

}

VramAuditViewModel::VramAuditViewModel(
    std::shared_ptr<application::vram::ServicoAuditarVramAtual> servico,
    std::shared_ptr<application::vram::ServicoRegistrarFeedback> feedback,
    QObject* parent)
    : QObject{parent}, m_servico{std::move(servico)}, m_feedback{std::move(feedback)} {}

void VramAuditViewModel::aplicar_relatorio(
    const application::vram::RelatorioAuditoriaVram& relatorio) {
    m_grupos = relatorio_to_variant(relatorio);
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
            m_grupos = relatorio_to_variant(relatorio);
            emit gruposChanged();
            m_busy.store(false);
            emit isBusyChanged();
        }, Qt::QueuedConnection);
    });
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): API Q_INVOKABLE recebe strings do QML; categoria é validada contra dois literais.
void VramAuditViewModel::registrarFeedback(const QString& chave_binario,
                                           const QString& categoria,
                                           const QString& motivo) {
    const auto veredito = categoria_de_feedback(categoria);
    const auto binario = chave_binario.trimmed();
    if (!m_feedback || !veredito || binario.isEmpty() || binario.size() > TAMANHO_MAXIMO_CHAVE) {
        emit feedbackRegistrado(false);
        return;
    }
    auto servico = m_feedback;
    auto chave = domain::vram::ChaveProcesso::de_binario(binario);
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
