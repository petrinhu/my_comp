#include "application/vram/auditoria/servico_auditar_vram_atual.h"

namespace vramopt::application {

ServicoAuditarVramAtual::ServicoAuditarVramAtual(
    std::shared_ptr<domain::IGpuBackend> backend,
    std::shared_ptr<ServicoClassificarProcesso> classificador,
    std::shared_ptr<ServicoSugerirAcao> sugeridor,
    std::shared_ptr<ServicoLocalizarConfig> localizador)
    : m_backend{std::move(backend)},
      m_classificador{std::move(classificador)},
      m_sugeridor{std::move(sugeridor)},
      m_localizador{std::move(localizador)} {}

RelatorioAuditoriaVram ServicoAuditarVramAtual::executar() const {
    RelatorioAuditoriaVram relatorio;

    const auto gpus = m_backend->enumerarGpus();
    relatorio.grupos.reserve(gpus.size());

    for (const auto& gpu : gpus) {
        GrupoAuditoriaGpu grupo{.gpu = gpu, .itens = {}};
        const auto processos = m_backend->enumerarProcessosVram(gpu.id());
        grupo.itens.reserve(processos.size());

        for (const auto& processo : processos) {
            ItemAuditoriaVram item{
                .processo      = processo,
                .classificacao = m_classificador->executar(processo),
                .sugestao      = {},
                .origens_config = m_localizador->executar(processo),
            };
            item.sugestao = m_sugeridor->sugerirPara(item.classificacao);
            grupo.itens.push_back(std::move(item));
        }

        relatorio.grupos.push_back(std::move(grupo));
    }

    return relatorio;
}

}
