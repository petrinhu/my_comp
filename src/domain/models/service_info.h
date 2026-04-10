#pragma once
#include <QString>

namespace my_comp::domain::models {

struct ServiceInfo {
    QString name;
    QString description;
    QString loadState;   // loaded, not-found, etc
    QString activeState; // active, inactive, failed
    QString subState;    // running, exited, dead
    bool isEnabled;      // enabled, disabled
};

} // namespace my_comp::domain::models
