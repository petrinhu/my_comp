#pragma once

#include "application/vram/auditoria/relatorio_auditoria_vram.h"
#include "application/vram/classificacao/servico_classificar_processo.h"
#include "application/vram/classificacao/servico_sugerir_acao.h"
#include "application/vram/configuracao/servico_localizar_config.h"
#include "domain/vram/interfaces/i_gpu_backend.h"

#include <memory>

namespace my_comp::application::vram {

class ServicoAuditarVramAtual {
public:
    ServicoAuditarVramAtual(
        std::shared_ptr<domain::vram::IGpuBackend> backend,
        std::shared_ptr<ServicoClassificarProcesso> classificador,
        std::shared_ptr<ServicoSugerirAcao> sugeridor,
        std::shared_ptr<ServicoLocalizarConfig> localizador);

    [[nodiscard]] RelatorioAuditoriaVram executar() const;

private:
    std::shared_ptr<domain::vram::IGpuBackend>        m_backend;
    std::shared_ptr<ServicoClassificarProcesso> m_classificador;
    std::shared_ptr<ServicoSugerirAcao>         m_sugeridor;
    std::shared_ptr<ServicoLocalizarConfig>     m_localizador;
};

}
