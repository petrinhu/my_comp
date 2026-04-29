#pragma once

#include "domain/vram/interfaces/i_config_locator.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockConfigLocator : public vramopt::domain::IConfigLocator {
public:
    MOCK_METHOD(QVector<vramopt::domain::OrigemConfig>, localizar,
                (const vramopt::domain::Processo&), (const, override));
};

}
