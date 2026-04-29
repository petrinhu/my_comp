#include "mid/classificacao/servico_classificar_processo.h"

namespace vramopt::application {

ServicoClassificarProcesso::ServicoClassificarProcesso(
    std::shared_ptr<domain::IClassificadorProcessos> classificador)
    : m_classificador{std::move(classificador)} {}

domain::ResultadoClassificacao
ServicoClassificarProcesso::executar(const domain::Processo& processo) const {
    return m_classificador->classificar(processo);
}

}
