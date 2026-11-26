#pragma once

#include "domain/vram/interfaces/i_leitor_desktop_exec.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockLeitorDesktopExec : public vramopt::domain::ILeitorDesktopExec {
public:
    MOCK_METHOD(std::optional<vramopt::domain::EntradaDesktopExec>, entrada_de,
                (const vramopt::domain::Processo&), (const, override));
};

}
