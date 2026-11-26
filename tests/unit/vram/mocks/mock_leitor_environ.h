#pragma once

#include "domain/vram/interfaces/i_leitor_environ.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockLeitorEnviron : public vramopt::domain::ILeitorEnviron {
public:
    MOCK_METHOD((QHash<QString, QString>), environ_de, (const vramopt::domain::PID&), (const, override));
};

}
