#include "system_info_viewmodel.h"
#include "ui_dispatch.h"

#include <QDebug>
#include <QPointer>
#include <QtConcurrent/QtConcurrent>

#include <exception>
#include <optional>
#include <utility>

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
        std::optional<domain::models::SystemInfo> info;
        try {
            info = provider->get_system_info();
        } catch (const std::exception& e) {
            qWarning() << "[SystemInfoVM] falha na coleta:" << e.what();
        } catch (...) {
            qWarning() << "[SystemInfoVM] falha desconhecida na coleta";
        }
        // Falha mantém os dados anteriores; só libera a trava.
        post_to_ui(self, [info = std::move(info)](SystemInfoViewModel* vm) {
            vm->m_busy = false;
            if (!info) return;
            vm->m_distro = (info->distroName + " " + info->distroVersion).trimmed();
            vm->m_kernel = info->kernelVersion;
            vm->m_driver = info->gpuDriver;
            emit vm->dataChanged();
        });
    });
}

} // namespace my_comp::presentation::viewmodels
