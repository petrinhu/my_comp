#ifndef UPDATE_PACKAGE_H
#define UPDATE_PACKAGE_H

#include <QString>

namespace my_comp::domain::models {

enum class ProviderType { Dnf, Flatpak, Npm, Snap, Cargo, Apt, Pacman };

struct UpdatePackage {
    QString name;
    QString current_version;
    QString new_version;
    QString source;
    ProviderType provider;
    QString description;
    QString source_repo;
    bool is_critical{false};

    bool operator==(const UpdatePackage& other) const {
        return name == other.name && source == other.source;
    }
};

} // namespace my_comp::domain::models

#endif // UPDATE_PACKAGE_H
