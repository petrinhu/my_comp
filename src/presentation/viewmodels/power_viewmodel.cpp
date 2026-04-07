#include "power_viewmodel.h"

namespace my_comp::presentation::viewmodels {
PowerViewModel::PowerViewModel(QObject* parent) : QObject(parent) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PowerViewModel::update);
    m_timer->start(3000);
    update();
}
void PowerViewModel::update() {
    auto stats = m_provider.get_power_data();
    if(!stats.empty()) {
        m_power = stats[0].power_watts;
        m_percent = stats[0].battery_percent;
        emit dataChanged();
    }
}
}
