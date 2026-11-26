#pragma once

#include "domain/vram/interfaces/i_leitor_desktop_file.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockLeitorDesktopFile : public my_comp::domain::vram::ILeitorDesktopFile {
public:
    MOCK_METHOD(QStringList, categorias_de, (const my_comp::domain::vram::Processo&), (const, override));
};

}
