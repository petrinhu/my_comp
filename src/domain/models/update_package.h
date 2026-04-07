#ifndef UPDATE_PACKAGE_H
#define UPDATE_PACKAGE_H

#include <QString>

namespace my_comp::domain::models {

struct UpdatePackage {
    QString name;
    QString current_version;
    QString new_version;
    QString source;      // e.g., "dnf", "flatpak", "npm"
    QString description;
    bool is_critical{false};

    bool operator==(const UpdatePackage& other) const {
        return name == other.name && source == other.source;
    }
};

} // namespace my_comp::domain::models

#endif // UPDATE_PACKAGE_H
