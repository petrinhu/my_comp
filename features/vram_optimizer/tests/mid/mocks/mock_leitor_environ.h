#pragma once

#include "back/interfaces/i_leitor_environ.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockLeitorEnviron : public vramopt::domain::ILeitorEnviron {
public:
    MOCK_METHOD((QHash<QString, QString>), environDe, (const vramopt::domain::PID&), (const, override));
};

}
