#include "presentation/cli/cli_vram.h"

#include "domain/vram/classificacao/categoria.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <vector>

namespace my_comp::presentation::cli {

namespace {

QString fabricante_string(domain::vram::Fabricante f) {
    switch (f) {
        case domain::vram::Fabricante::Nvidia:       return QStringLiteral("Nvidia");
        case domain::vram::Fabricante::Intel:        return QStringLiteral("Intel");
        case domain::vram::Fabricante::Amd:          return QStringLiteral("AMD");
        case domain::vram::Fabricante::Desconhecido: return QStringLiteral("?");
    }
    return QStringLiteral("?");
}

QString categoria_string(domain::vram::Categoria c) {
    switch (c) {
        case domain::vram::Categoria::CandidatoObvioCpu: return QStringLiteral("cpu");
        case domain::vram::Categoria::Indeterminado:     return QStringLiteral("indeterminado");
        case domain::vram::Categoria::PrecisaGpu:        return QStringLiteral("gpu");
    }
    return QStringLiteral("?");
}

QJsonObject origem_to_json(const domain::vram::OrigemConfig& o) {
    QJsonObject j;
    j[QStringLiteral("tipo")] = o.tipo;
    j[QStringLiteral("caminho")] = o.caminho;
    j[QStringLiteral("chave")] = o.chave;
    j[QStringLiteral("valor_atual")] = o.valor_atual;
    j[QStringLiteral("valor_sugerido")] = o.valor_sugerido;
    j[QStringLiteral("instrucao")] = o.instrucao_humana;
    return j;
}

QString formatar_json(const application::vram::RelatorioAuditoriaVram& rel,
                     const OpcoesCliVram& opts) {
    QJsonArray gpus;
    for (const auto& grupo : rel.grupos) {
        QJsonObject g;
        g[QStringLiteral("gpu_id")] = grupo.gpu.id();
        g[QStringLiteral("fabricante")] = fabricante_string(grupo.gpu.fabricante());
        g[QStringLiteral("modelo")] = grupo.gpu.modelo();
        g[QStringLiteral("vram_total_bytes")] =
            static_cast<qint64>(grupo.gpu.vram_total().valor());

        QJsonArray itens;
        for (const auto& item : grupo.itens) {
            QJsonObject it;
            it[QStringLiteral("pid")] = item.processo.pid().valor();
            it[QStringLiteral("nome")] = item.processo.nome().valor();
            it[QStringLiteral("vram_bytes")] =
                static_cast<qint64>(item.processo.vram_consumida().valor());
            it[QStringLiteral("categoria")] =
                categoria_string(item.classificacao.categoria);
            it[QStringLiteral("regra")] = item.classificacao.justificativa.regra;

            if (opts.include_config_origins && !item.origens_config.isEmpty()) {
                QJsonArray origens;
                for (const auto& o : item.origens_config) {
                    origens.append(origem_to_json(o));
                }
                it[QStringLiteral("origens_config")] = origens;
            }
            itens.append(it);
        }
        g[QStringLiteral("itens")] = itens;
        gpus.append(g);
    }
    QJsonObject root;
    root[QStringLiteral("gpus")] = gpus;
    return QString::fromUtf8(QJsonDocument{root}.toJson(QJsonDocument::Indented));
}

QString formatar_tabular(const application::vram::RelatorioAuditoriaVram& rel,
                        const OpcoesCliVram& opts) {
    QString out;
    if (rel.grupos.empty()) {
        out += QStringLiteral("Nenhuma GPU detectada.\n");
        return out;
    }
    for (const auto& grupo : rel.grupos) {
        out += QStringLiteral("\n=== GPU %1: %2 (%3) — VRAM total: ")
                   .arg(grupo.gpu.id())
                   .arg(fabricante_string(grupo.gpu.fabricante()),
                        grupo.gpu.modelo());
        const auto vram_total_mb = grupo.gpu.vram_total().valor() / (1024ULL * 1024ULL);
        if (vram_total_mb > 0) {
            out += QStringLiteral("%1 MiB ===\n").arg(vram_total_mb);
        } else {
            out += QStringLiteral("(desconhecida) ===\n");
        }
        if (grupo.itens.empty()) {
            out += QStringLiteral("  (nenhum processo usando VRAM nesta GPU)\n");
            continue;
        }
        out += QStringLiteral("  %1  %2  %3  %4\n")
                   .arg(QStringLiteral("PID"), -7)
                   .arg(QStringLiteral("NOME"), -24)
                   .arg(QStringLiteral("VRAM (MiB)"), -12)
                   .arg(QStringLiteral("CATEGORIA"));
        for (const auto& item : grupo.itens) {
            const auto vram_mb = item.processo.vram_consumida().valor() / (1024ULL * 1024ULL);
            out += QStringLiteral("  %1  %2  %3  %4\n")
                       .arg(item.processo.pid().valor(), -7)
                       .arg(item.processo.nome().valor(), -24)
                       .arg(vram_mb, -12)
                       .arg(categoria_string(item.classificacao.categoria));
            if (opts.include_config_origins) {
                for (const auto& o : item.origens_config) {
                    out += QStringLiteral("      ↳ [%1] %2 = %3 → sugerido: %4\n")
                               .arg(o.tipo, o.chave, o.valor_atual, o.valor_sugerido);
                }
            }
        }
    }
    return out;
}

}

OpcoesCliVram parsear_opcoes(const QStringList& args) {
    OpcoesCliVram opts;
    for (const auto& a : args) {
        if (a == QStringLiteral("--json")) {
            opts.json = true;
        } else if (a == QStringLiteral("--all-processes")) {
            opts.all_processes = true;
        } else if (a == QStringLiteral("--include-config-origins")) {
            opts.include_config_origins = true;
        } else if (a == QStringLiteral("--no-config-origins")) {
            opts.include_config_origins = false;
        }
    }
    return opts;
}

QString formatar_relatorio(const application::vram::RelatorioAuditoriaVram& rel,
                          const OpcoesCliVram& opts) {
    return opts.json ? formatar_json(rel, opts) : formatar_tabular(rel, opts);
}

int executar_com_servico(
    const std::shared_ptr<application::vram::ServicoAuditarVramAtual>& servico,
    const QStringList& args,
    QTextStream& out) {
    if (!servico) {
        out << "Erro: serviço de auditoria nulo.\n";
        return 2;
    }
    const auto opts = parsear_opcoes(args);
    const auto relatorio = servico->executar();
    out << formatar_relatorio(relatorio, opts);
    if (!opts.json) {
        out << "\n";
    }
    return 0;
}


}
