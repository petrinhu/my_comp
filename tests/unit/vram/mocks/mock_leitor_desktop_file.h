#pragma once

#include "domain/vram/interfaces/i_leitor_desktop_file.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockLeitorDesktopFile : public vramopt::domain::ILeitorDesktopFile {
public:
    MOCK_METHOD(QStringList, categorias_de, (const vramopt::domain::Processo&), (const, override));
};

}
