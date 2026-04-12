#include "power_viewmodel.h"

#include "../../infrastructure/collectors/linux_power_provider.h"

namespace my_comp::presentation::viewmodels {

PowerViewModel::PowerViewModel(QObject* parent) 
    : QObject(parent), m_provider(std::make_shared<infrastructure::collectors::LinuxPowerProvider>()) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PowerViewModel::update);
    m_timer->start(2000);
    update();
}

PowerViewModel::PowerViewModel(std::shared_ptr<domain::interfaces::IPowerProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PowerViewModel::update);
    m_timer->start(2000);
    update();
}

void PowerViewModel::update() {
    auto stats = m_provider->collect_stats();
    
    bool changed = false;
    if (m_power != stats.consumption_watts) { m_power = stats.consumption_watts; changed = true; }
    if (m_percent != stats.battery_percent) { m_percent = stats.battery_percent; changed = true; }
    
    if (changed) {
        emit dataChanged();
    }
}

}
