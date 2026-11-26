#pragma once
#include <QString>

namespace my_comp::domain::models {

// Instância do systemd a que o unit pertence (systemctl vs systemctl --user).
enum class ServiceScope { System, User };

struct ServiceInfo {
    QString name;        // nome do unit, sem metadado visual (SV-01b)
    QString description;
    QString loadState;   // loaded, not-found, etc
    QString activeState; // active, inactive, failed
    QString subState;    // running, exited, dead
    bool isEnabled = false;
    ServiceScope scope = ServiceScope::System;
};

} // namespace my_comp::domain::models
