#include "mid/configuracao/localizador_composite.h"

namespace vramopt::application {

LocalizadorComposite::LocalizadorComposite(
    std::vector<std::shared_ptr<domain::IConfigLocator>> sublocators)
    : m_sublocators{std::move(sublocators)} {}

QVector<domain::OrigemConfig>
LocalizadorComposite::localizar(const domain::Processo& processo) const {
    QVector<domain::OrigemConfig> agregadas;
    for (const auto& sub : m_sublocators) {
        const auto parciais = sub->localizar(processo);
        for (const auto& o : parciais) {
            agregadas.append(o);
        }
    }
    return agregadas;
}

}
