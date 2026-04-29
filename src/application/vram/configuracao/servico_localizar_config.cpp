#include "application/vram/configuracao/servico_localizar_config.h"

namespace vramopt::application {

ServicoLocalizarConfig::ServicoLocalizarConfig(
    std::shared_ptr<domain::IConfigLocator> locator)
    : m_locator{std::move(locator)} {}

QVector<domain::OrigemConfig>
ServicoLocalizarConfig::executar(const domain::Processo& processo) const {
    return m_locator->localizar(processo);
}

}
