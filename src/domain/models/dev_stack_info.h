#pragma once
#include <QString>
#include <vector>

namespace my_comp::domain::models {

struct ToolchainInfo {
    QString name;
    QString version;
    bool isInstalled;
};

struct ContainerInfo {
    QString id;
    QString name;
    QString image;
    QString status;
    QString runtime; // Podman ou Docker
};

struct DevStackInfo {
    std::vector<ToolchainInfo> toolchains;
    std::vector<ContainerInfo> containers;
};

} // namespace my_comp::domain::models
