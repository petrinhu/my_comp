#pragma once

#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"

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
