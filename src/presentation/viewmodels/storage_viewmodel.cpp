#include "storage_viewmodel.h"
#include <QLocale>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>
#include <QPointer>

namespace my_comp::presentation::viewmodels {

StorageViewModel::StorageViewModel(std::shared_ptr<domain::interfaces::IStorageProvider> provider,
                                   std::shared_ptr<domain::interfaces::IDiskInventoryProvider> inventory_provider,
                                   std::shared_ptr<domain::interfaces::ISmartProvider> smart_provider,
                                   std::shared_ptr<domain::interfaces::IBenchmarkProvider> benchmark_provider,
                                   QObject *parent)
    : QObject(parent), m_provider(provider),
      m_inventory_provider(std::move(inventory_provider)),
      m_smart_provider(std::move(smart_provider)),
      m_benchmark_provider(std::move(benchmark_provider)) {

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &StorageViewModel::refresh);
}

void StorageViewModel::initialize() {
    refreshInventory();
}

void StorageViewModel::refresh() {
    if (!m_timer->isActive()) {
        m_timer->start(1000);
    }
    if (m_busy) return;
    m_busy = true;

    const QPointer<StorageViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        auto stats = provider->collect_stats();
        QMetaObject::invokeMethod(self.data(), [self, stats]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_stats = stats;
            vm->m_busy = false;
            emit vm->statsChanged();
        }, Qt::QueuedConnection);
    });
}

void StorageViewModel::refreshInventory() {
    if (!m_inventory_provider || m_inventory_busy) return;
    m_inventory_busy = true;

    const QPointer<StorageViewModel> self(this);
    auto provider = m_inventory_provider;
    auto smart = m_smart_provider;
    (void)QtConcurrent::run([self, provider, smart]() {
        auto inv = provider->collect_inventory();
        std::vector<domain::models::SmartReport> smart_reports(inv.drives.size());
        if (smart) {
            // `smartctl` pode demorar; roda serialmente dentro do worker (já off-thread).
            for (size_t i = 0; i < inv.drives.size(); ++i) {
                smart_reports[i] = smart->collect_report(inv.drives[i].name);
            }
        }
        QMetaObject::invokeMethod(self.data(), [self, inv, smart_reports]() {
            auto* vm = self.data();
            if (!vm) return;
            QVariantList list;
            list.reserve(static_cast<int>(inv.drives.size()));
            for (size_t i = 0; i < inv.drives.size(); ++i) {
                const auto& d = inv.drives[i];
                QVariantMap drive;
                drive["name"] = d.name;
                drive["sizeBytes"] = static_cast<qlonglong>(d.size_bytes);
                drive["type"] = d.type;
                drive["vendor"] = d.vendor;
                drive["model"] = d.model;
                drive["rotational"] = d.rotational;

                QVariantMap smart_map;
                if (i < smart_reports.size()) {
                    const auto& r = smart_reports[i];
                    smart_map["available"] = r.available;
                    smart_map["passed"] = r.passed;
                    smart_map["tempC"] = r.temp_c;
                    smart_map["powerOnHours"] = static_cast<qlonglong>(r.power_on_hours);
                    smart_map["wearPct"] = r.wear_pct;
                    smart_map["errorMessage"] = r.error_message;
                } else {
                    smart_map["available"] = false;
                }
                drive["smart"] = smart_map;

                QVariantList parts;
                parts.reserve(static_cast<int>(d.partitions.size()));
                for (const auto& p : d.partitions) {
                    QVariantMap pm;
                    pm["name"] = p.name;
                    pm["mount"] = p.mount;
                    pm["fs"] = p.fs;
                    pm["sizeBytes"] = static_cast<qlonglong>(p.size_bytes);
                    pm["label"] = p.label;
                    pm["uuid"] = p.uuid;
                    pm["usedBytes"] = static_cast<qlonglong>(p.used_bytes);
                    pm["freeBytes"] = static_cast<qlonglong>(p.free_bytes);
                    pm["usedPercent"] = p.used_percent;
                    parts.append(pm);
                }
                drive["partitions"] = parts;
                list.append(drive);
            }
            vm->m_drives = list;
            vm->m_inventory_busy = false;
            emit vm->inventoryChanged();
        }, Qt::QueuedConnection);
    });
}

void StorageViewModel::runBenchmark(const QString& deviceName) {
    if (!m_benchmark_provider || m_benchmarking) return;
    m_benchmarking = true;
    m_last_benchmark.clear();
    m_last_benchmark["device"] = deviceName;
    m_last_benchmark["running"] = true;
    emit benchmarkChanged();

    const QPointer<StorageViewModel> self(this);
    auto provider = m_benchmark_provider;
    (void)QtConcurrent::run([self, provider, deviceName]() {
        auto result = provider->run(deviceName);
        QMetaObject::invokeMethod(self.data(), [self, result]() {
            auto* vm = self.data();
            if (!vm) return;
            QVariantMap m;
            m["device"] = result.device;
            m["seqReadMbS"] = result.seq_read_mb_s;
            m["cachedMbS"] = result.cached_mb_s;
            m["durationS"] = result.duration_s;
            m["success"] = result.success;
            m["errorMessage"] = result.error_message;
            m["running"] = false;
            vm->m_last_benchmark = m;
            vm->m_benchmarking = false;
            emit vm->benchmarkChanged();
        }, Qt::QueuedConnection);
    });
}

QString StorageViewModel::totalThroughput() const {
    const double total = m_stats.total_read_mb_s + m_stats.total_write_mb_s;
    if (total < 0.1) return "0 MB/s";
    return QString("%1 MB/s").arg(QLocale().toString(total, 'f', 1));
}

} // namespace my_comp::presentation::viewmodels
