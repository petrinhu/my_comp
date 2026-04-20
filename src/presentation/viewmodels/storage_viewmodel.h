#pragma once
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <memory>
#include "../../domain/interfaces/i_storage_provider.h"
#include "../../domain/interfaces/i_disk_inventory_provider.h"
#include "../../domain/interfaces/i_smart_provider.h"
#include "../../domain/interfaces/i_benchmark_provider.h"

namespace my_comp::presentation::viewmodels {

class StorageViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(double totalReadMb READ totalReadMb NOTIFY statsChanged)
    Q_PROPERTY(double totalWriteMb READ totalWriteMb NOTIFY statsChanged)
    Q_PROPERTY(QString totalThroughput READ totalThroughput NOTIFY statsChanged)
    Q_PROPERTY(QVariantList drives READ drives NOTIFY inventoryChanged)
    Q_PROPERTY(bool isBenchmarking READ isBenchmarking NOTIFY benchmarkChanged)
    Q_PROPERTY(QVariantMap lastBenchmark READ lastBenchmark NOTIFY benchmarkChanged)

public:
    explicit StorageViewModel(std::shared_ptr<domain::interfaces::IStorageProvider> provider,
                              std::shared_ptr<domain::interfaces::IDiskInventoryProvider> inventory_provider = nullptr,
                              std::shared_ptr<domain::interfaces::ISmartProvider> smart_provider = nullptr,
                              std::shared_ptr<domain::interfaces::IBenchmarkProvider> benchmark_provider = nullptr,
                              QObject *parent = nullptr);

    double totalReadMb() const { return m_stats.total_read_mb_s; }
    double totalWriteMb() const { return m_stats.total_write_mb_s; }
    QString totalThroughput() const;
    QVariantList drives() const { return m_drives; }
    bool isBenchmarking() const { return m_benchmarking; }
    QVariantMap lastBenchmark() const { return m_last_benchmark; }

    Q_INVOKABLE void initialize();
    Q_INVOKABLE void refreshInventory();
    Q_INVOKABLE void runBenchmark(const QString& deviceName);

public slots:
    void refresh();

signals:
    void statsChanged();
    void inventoryChanged();
    void benchmarkChanged();

private:
    std::shared_ptr<domain::interfaces::IStorageProvider> m_provider;
    std::shared_ptr<domain::interfaces::IDiskInventoryProvider> m_inventory_provider;
    std::shared_ptr<domain::interfaces::ISmartProvider> m_smart_provider;
    std::shared_ptr<domain::interfaces::IBenchmarkProvider> m_benchmark_provider;
    domain::models::StorageStats m_stats;
    QVariantList m_drives;
    QVariantMap m_last_benchmark;
    QTimer *m_timer;
    bool m_busy = false;
    bool m_inventory_busy = false;
    bool m_benchmarking = false;
};

} // namespace my_comp::presentation::viewmodels
