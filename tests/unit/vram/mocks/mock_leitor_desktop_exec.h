#pragma once

#include "domain/vram/interfaces/i_leitor_desktop_exec.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockLeitorDesktopExec : public my_comp::domain::vram::ILeitorDesktopExec {
public:
    MOCK_METHOD(std::optional<my_comp::domain::vram::EntradaDesktopExec>, entrada_de,
                (const my_comp::domain::vram::Processo&), (const, override));
};

}
