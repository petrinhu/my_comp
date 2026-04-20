#include "storage_viewmodel.h"
#include <QLocale>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>
#include <QPointer>

namespace my_comp::presentation::viewmodels {

StorageViewModel::StorageViewModel(std::shared_ptr<domain::interfaces::IStorageProvider> provider,
                                   std::shared_ptr<domain::interfaces::IDiskInventoryProvider> inventory_provider,
                                   QObject *parent)
    : QObject(parent), m_provider(provider), m_inventory_provider(std::move(inventory_provider)) {

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

    QPointer<StorageViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        auto stats = provider->collect_stats();
        QMetaObject::invokeMethod(self.data(), [self, stats]() {
            if (!self) return;
            self->m_stats = stats;
            self->m_busy = false;
            emit self->statsChanged();
        }, Qt::QueuedConnection);
    });
}

void StorageViewModel::refreshInventory() {
    if (!m_inventory_provider || m_inventory_busy) return;
    m_inventory_busy = true;

    QPointer<StorageViewModel> self(this);
    auto provider = m_inventory_provider;
    (void)QtConcurrent::run([self, provider]() {
        auto inv = provider->collect_inventory();
        QMetaObject::invokeMethod(self.data(), [self, inv]() {
            if (!self) return;
            QVariantList list;
            list.reserve(static_cast<int>(inv.drives.size()));
            for (const auto& d : inv.drives) {
                QVariantMap drive;
                drive["name"] = d.name;
                drive["sizeBytes"] = static_cast<qlonglong>(d.size_bytes);
                drive["type"] = d.type;
                drive["vendor"] = d.vendor;
                drive["model"] = d.model;
                drive["rotational"] = d.rotational;

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
            self->m_drives = list;
            self->m_inventory_busy = false;
            emit self->inventoryChanged();
        }, Qt::QueuedConnection);
    });
}

QString StorageViewModel::totalThroughput() const {
    double total = m_stats.total_read_mb_s + m_stats.total_write_mb_s;
    if (total < 0.1) return "0 MB/s";
    return QString("%1 MB/s").arg(QLocale().toString(total, 'f', 1));
}

} // namespace my_comp::presentation::viewmodels
