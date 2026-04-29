#pragma once

#include "back/configuracao/origem_config.h"
#include "back/entidades/processo.h"

#include <QVector>

namespace vramopt::domain {

class IConfigLocator {
public:
    IConfigLocator() = default;
    IConfigLocator(const IConfigLocator&) = delete;
    IConfigLocator(IConfigLocator&&) = delete;
    IConfigLocator& operator=(const IConfigLocator&) = delete;
    IConfigLocator& operator=(IConfigLocator&&) = delete;
    virtual ~IConfigLocator() = default;

    [[nodiscard]] virtual QVector<OrigemConfig>
        localizar(const Processo& processo) const = 0;
};

}
