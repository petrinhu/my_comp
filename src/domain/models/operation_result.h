#pragma once
#include <QString>

namespace my_comp::domain::models {

// Resultado de uma operação de sistema com mensagem para o usuário: o `bool` sozinho
// obrigava a UI a mostrar "Falha" genérico sem dizer o que corrigir.
struct OperationResult {
    bool ok = false;
    QString message;
};

} // namespace my_comp::domain::models
