#pragma once
#include <QObject>
#include <QTimer>
#include "../../infrastructure/collectors/linux_power_provider.h"

namespace my_comp::presentation::viewmodels {
class PowerViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(double powerWatts READ powerWatts NOTIFY dataChanged)
    Q_PROPERTY(double batteryPercent READ batteryPercent NOTIFY dataChanged)
public:
    PowerViewModel(QObject* parent = nullptr);
    double powerWatts() const { return m_power; }
    double batteryPercent() const { return m_percent; }
signals:
    void dataChanged();
private slots:
    void update();
private:
    infrastructure::collectors::LinuxPowerProvider m_provider;
    double m_power = 0.0;
    double m_percent = 0.0;
    QTimer* m_timer;
};
}
