#pragma once
#include <QString>

namespace my_comp::domain::models {

struct SystemInfo {
    QString distroName;
    QString distroVersion;
    QString kernelVersion;
    QString gpuDriver;
    bool kernelUpdateAvailable = false;
};

} // namespace my_comp::domain::models
