#pragma once

#include "domain/vram/interfaces/i_repositorio_feedback.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockRepositorioFeedback : public my_comp::domain::vram::IRepositorioFeedback {
public:
    MOCK_METHOD(std::optional<my_comp::domain::vram::EntradaFeedback>, buscar,
                (const my_comp::domain::vram::ChaveProcesso&), (const, override));
    MOCK_METHOD(void, inserir, (const my_comp::domain::vram::EntradaFeedback&), (override));
    MOCK_METHOD(void, atualizar_visto_em, (int, QDateTime), (override));
    MOCK_METHOD(void, remover, (int), (override));
    MOCK_METHOD(QVector<my_comp::domain::vram::EntradaFeedback>, listar_todos, (), (const, override));
};

}
