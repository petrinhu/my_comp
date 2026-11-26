#pragma once

#include "domain/vram/interfaces/i_leitor_environ.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockLeitorEnviron : public my_comp::domain::vram::ILeitorEnviron {
public:
    MOCK_METHOD((QHash<QString, QString>), environ_de, (const my_comp::domain::vram::PID&), (const, override));
};

}
