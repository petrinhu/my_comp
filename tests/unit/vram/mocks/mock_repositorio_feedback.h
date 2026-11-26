#pragma once

#include "domain/vram/interfaces/i_repositorio_feedback.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockRepositorioFeedback : public vramopt::domain::IRepositorioFeedback {
public:
    MOCK_METHOD(std::optional<vramopt::domain::EntradaFeedback>, buscar,
                (const vramopt::domain::ChaveProcesso&), (const, override));
    MOCK_METHOD(void, inserir, (const vramopt::domain::EntradaFeedback&), (override));
    MOCK_METHOD(void, atualizar_visto_em, (int, QDateTime), (override));
    MOCK_METHOD(void, remover, (int), (override));
    MOCK_METHOD(QVector<vramopt::domain::EntradaFeedback>, listar_todos, (), (const, override));
};

}
