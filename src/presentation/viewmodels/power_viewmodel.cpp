#include "power_viewmodel.h"
#include "ui_dispatch.h"

#include <QDebug>
#include <QPointer>
#include <QtConcurrent/QtConcurrent>

#include <exception>
#include <optional>
#include <utility>

namespace my_comp::presentation::viewmodels {

// Provider injetado pelo composition root (MC-02). MC-04: ctor sem I/O e sem start de timer.
PowerViewModel::PowerViewModel(std::shared_ptr<domain::interfaces::IPowerProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PowerViewModel::update);
}

void PowerViewModel::initialize() {
    if (m_initialized) return;
    m_initialized = true;
    update();
    m_timer->start(2000);
}

// UX-05: collect_stats lê ~9 arquivos em /sys/class/power_supply a cada 2 s: fora da
// thread da UI. Se a coleta anterior ainda não voltou, o tique do timer é pulado; uma
// falha só perde aquele tique (QA-13).
void PowerViewModel::update() {
    if (!m_provider || m_busy) return;
    m_busy = true;

    const QPointer<PowerViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        std::optional<domain::models::PowerStats> stats;
        try {
            stats = provider->collect_stats();
        } catch (const std::exception& e) {
            qWarning() << "[PowerVM] falha na coleta:" << e.what();
        } catch (...) {
            qWarning() << "[PowerVM] falha desconhecida na coleta";
        }
        post_to_ui(self, [stats = std::move(stats)](PowerViewModel* vm) {
            vm->m_busy = false;
            if (!stats) return;
            bool changed = false;
            if (vm->m_power != stats->consumption_watts) { vm->m_power = stats->consumption_watts; changed = true; }
            if (vm->m_percent != stats->battery_percent) { vm->m_percent = stats->battery_percent; changed = true; }
            if (changed) {
                emit vm->dataChanged();
            }
        });
    });
}

}
