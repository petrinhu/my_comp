#include "power_viewmodel.h"

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

void PowerViewModel::update() {
    if (!m_provider) return;
    auto stats = m_provider->collect_stats();

    bool changed = false;
    if (m_power != stats.consumption_watts) { m_power = stats.consumption_watts; changed = true; }
    if (m_percent != stats.battery_percent) { m_percent = stats.battery_percent; changed = true; }
    
    if (changed) {
        emit dataChanged();
    }
}

}
