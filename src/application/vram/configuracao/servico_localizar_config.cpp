#include "application/vram/configuracao/servico_localizar_config.h"

namespace my_comp::application::vram {

ServicoLocalizarConfig::ServicoLocalizarConfig(
    std::shared_ptr<domain::vram::IConfigLocator> locator)
    : m_locator{std::move(locator)} {}

QVector<domain::vram::OrigemConfig>
ServicoLocalizarConfig::executar(const domain::vram::Processo& processo) const {
    return m_locator->localizar(processo);
}

}
