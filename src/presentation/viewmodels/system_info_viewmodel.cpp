#include "system_info_viewmodel.h"

#include <QDebug>
#include <QMetaObject>
#include <QPointer>
#include <QtConcurrent/QtConcurrent>

#include <exception>

namespace my_comp::presentation::viewmodels {

SystemInfoViewModel::SystemInfoViewModel(std::shared_ptr<domain::interfaces::ISystemInfoProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
}

// UX-05: get_system_info lê /etc/os-release e roda uname e lspci (QProcess bloqueante):
// fora da thread da UI. Uma falha não pode deixar m_busy preso (QA-13).
void SystemInfoViewModel::refresh() {
    if (!m_provider || m_busy) return;
    m_busy = true;

    const QPointer<SystemInfoViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        domain::models::SystemInfo info;
        try {
            info = provider->get_system_info();
        } catch (const std::exception& e) {
            qWarning() << "[SystemInfoVM] falha na coleta:" << e.what();
        } catch (...) {
            qWarning() << "[SystemInfoVM] falha desconhecida na coleta";
        }
        QMetaObject::invokeMethod(self.data(), [self, info]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_distro = (info.distroName + " " + info.distroVersion).trimmed();
            vm->m_kernel = info.kernelVersion;
            vm->m_driver = info.gpuDriver;
            vm->m_busy = false;
            emit vm->dataChanged();
        }, Qt::QueuedConnection);
    });
}

} // namespace my_comp::presentation::viewmodels
