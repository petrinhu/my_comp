#pragma once
#include <QObject>
#include <QTimer>
#include "../../domain/interfaces/i_power_provider.h"
#include <memory>

namespace my_comp::presentation::viewmodels {
class PowerViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(double powerWatts READ powerWatts NOTIFY dataChanged)
    Q_PROPERTY(double batteryPercent READ batteryPercent NOTIFY dataChanged)
public:
    PowerViewModel(std::shared_ptr<domain::interfaces::IPowerProvider> provider, QObject* parent = nullptr);
    explicit PowerViewModel(QObject* parent = nullptr);
    double powerWatts() const { return m_power; }
    double batteryPercent() const { return m_percent; }

    Q_INVOKABLE void initialize();
signals:
    void dataChanged();
private slots:
    void update();
private:
    std::shared_ptr<domain::interfaces::IPowerProvider> m_provider;
    double m_power = 0.0;
    double m_percent = 0.0;
    QTimer* m_timer;
    bool m_initialized = false;
};
}
