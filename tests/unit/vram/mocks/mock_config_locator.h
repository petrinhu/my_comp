#pragma once

#include "domain/vram/interfaces/i_config_locator.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockConfigLocator : public my_comp::domain::vram::IConfigLocator {
public:
    MOCK_METHOD(QVector<my_comp::domain::vram::OrigemConfig>, localizar,
                (const my_comp::domain::vram::Processo&), (const, override));
};

}
