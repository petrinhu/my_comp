#include "application/vram/configuracao/localizador_composite.h"

namespace my_comp::application::vram {

LocalizadorComposite::LocalizadorComposite(
    std::vector<std::shared_ptr<domain::vram::IConfigLocator>> sublocators)
    : m_sublocators{std::move(sublocators)} {}

QVector<domain::vram::OrigemConfig>
LocalizadorComposite::localizar(const domain::vram::Processo& processo) const {
    QVector<domain::vram::OrigemConfig> agregadas;
    for (const auto& sub : m_sublocators) {
        const auto parciais = sub->localizar(processo);
        for (const auto& o : parciais) {
            agregadas.append(o);
        }
    }
    return agregadas;
}

}
