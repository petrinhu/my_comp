#pragma once
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <memory>
#include "../../domain/interfaces/i_cpu_provider.h"

namespace my_comp::presentation::viewmodels {

class CpuViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(double totalLoad READ totalLoad NOTIFY statsChanged)
    Q_PROPERTY(double avgFrequency READ avgFrequency NOTIFY statsChanged)
    Q_PROPERTY(double temperature READ temperature NOTIFY statsChanged)
    Q_PROPERTY(QVariantList coreLoads READ coreLoads NOTIFY statsChanged)
    
    Q_PROPERTY(QString modelName READ modelName CONSTANT)
    Q_PROPERTY(QString vendorId READ vendorId CONSTANT)
    Q_PROPERTY(int physicalCores READ physicalCores CONSTANT)
    Q_PROPERTY(int logicalThreads READ logicalThreads CONSTANT)
    Q_PROPERTY(QString l1Cache READ l1Cache CONSTANT)
    Q_PROPERTY(QString l2Cache READ l2Cache CONSTANT)
    Q_PROPERTY(QString l3Cache READ l3Cache CONSTANT)

public:
    explicit CpuViewModel(std::shared_ptr<domain::interfaces::ICpuProvider> provider, QObject *parent = nullptr);

    double totalLoad() const { return m_stats.total_load; }
    double avgFrequency() const { return m_stats.average_frequency; }
    double temperature() const { return m_stats.temperature; }
    QVariantList coreLoads() const;

    QString modelName() const { return QString::fromStdString(m_arch.model_name); }
    QString vendorId() const { return QString::fromStdString(m_arch.vendor_id); }
    int physicalCores() const { return m_arch.physical_cores; }
    int logicalThreads() const { return m_arch.logical_threads; }
    QString l1Cache() const { return QString::fromStdString(m_arch.l1_cache); }
    QString l2Cache() const { return QString::fromStdString(m_arch.l2_cache); }
    QString l3Cache() const { return QString::fromStdString(m_arch.l3_cache); }

public slots:
    void refresh();

signals:
    void statsChanged();

private:
    std::shared_ptr<domain::interfaces::ICpuProvider> m_provider;
    domain::models::CpuStats m_stats;
    domain::models::CpuArchitecture m_arch;
    QTimer *m_timer;
};

} // namespace my_comp::presentation::viewmodels
