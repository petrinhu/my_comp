#include "application/vram/classificacao/servico_classificar_processo.h"

namespace my_comp::application::vram {

ServicoClassificarProcesso::ServicoClassificarProcesso(
    std::shared_ptr<domain::vram::IClassificadorProcessos> classificador)
    : m_classificador{std::move(classificador)} {}

domain::vram::ResultadoClassificacao
ServicoClassificarProcesso::executar(const domain::vram::Processo& processo) const {
    return m_classificador->classificar(processo);
}

}
