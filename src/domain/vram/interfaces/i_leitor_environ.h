#pragma once

#include "domain/vram/valueobjects/pid.h"

#include <QHash>
#include <QString>

namespace vramopt::domain {

class ILeitorEnviron {
public:
    ILeitorEnviron() = default;
    ILeitorEnviron(const ILeitorEnviron&) = delete;
    ILeitorEnviron(ILeitorEnviron&&) = delete;
    ILeitorEnviron& operator=(const ILeitorEnviron&) = delete;
    ILeitorEnviron& operator=(ILeitorEnviron&&) = delete;
    virtual ~ILeitorEnviron() = default;

    [[nodiscard]] virtual QHash<QString, QString>
        environDe(const PID& pid) const = 0;
};

}
