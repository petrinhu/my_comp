#pragma once

// Infra compartilhada entre os módulos de composição (MC-02: só o composition root a cria).

#include "domain/interfaces/i_preferences_store.h"
#include "infrastructure/collectors/i_shell_executor.h"

#include <memory>

namespace my_comp::composition {

struct SharedInfrastructure {
    std::shared_ptr<infrastructure::collectors::IShellExecutor> shell;
    std::shared_ptr<domain::interfaces::IPreferencesStore> preferences;
};

[[nodiscard]] SharedInfrastructure compor_infra_compartilhada();

// Procura no PATH sem spawnar processo (detecção de ferramentas opcionais na inicialização).
[[nodiscard]] bool ferramenta_disponivel(const QString& nome);

}
