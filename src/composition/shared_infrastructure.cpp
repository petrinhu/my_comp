#include "composition/shared_infrastructure.h"

#include "infrastructure/collectors/qprocess_shell_executor.h"

#include <QStandardPaths>

namespace my_comp::composition {

SharedInfrastructure compor_infra_compartilhada() {
    return {.shell = std::make_shared<infrastructure::collectors::QProcessShellExecutor>()};
}

bool ferramenta_disponivel(const QString& nome) {
    // /usr/sbin nem sempre está no PATH do usuário (smartctl, hdparm).
    return !QStandardPaths::findExecutable(nome).isEmpty()
        || !QStandardPaths::findExecutable(nome, {QStringLiteral("/usr/sbin"), QStringLiteral("/sbin")}).isEmpty();
}

}
