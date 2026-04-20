#pragma once
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <memory>
#include "../../domain/interfaces/i_storage_provider.h"
#include "../../domain/interfaces/i_disk_inventory_provider.h"
#include "../../domain/interfaces/i_smart_provider.h"

namespace my_comp::presentation::viewmodels {

class StorageViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(double totalReadMb READ totalReadMb NOTIFY statsChanged)
    Q_PROPERTY(double totalWriteMb READ totalWriteMb NOTIFY statsChanged)
    Q_PROPERTY(QString totalThroughput READ totalThroughput NOTIFY statsChanged)
    Q_PROPERTY(QVariantList drives READ drives NOTIFY inventoryChanged)

public:
    explicit StorageViewModel(std::shared_ptr<domain::interfaces::IStorageProvider> provider,
                              std::shared_ptr<domain::interfaces::IDiskInventoryProvider> inventory_provider = nullptr,
                              std::shared_ptr<domain::interfaces::ISmartProvider> smart_provider = nullptr,
                              QObject *parent = nullptr);

    double totalReadMb() const { return m_stats.total_read_mb_s; }
    double totalWriteMb() const { return m_stats.total_write_mb_s; }
    QString totalThroughput() const;
    QVariantList drives() const { return m_drives; }

    Q_INVOKABLE void initialize();
    Q_INVOKABLE void refreshInventory();

public slots:
    void refresh();

signals:
    void statsChanged();
    void inventoryChanged();

private:
    std::shared_ptr<domain::interfaces::IStorageProvider> m_provider;
    std::shared_ptr<domain::interfaces::IDiskInventoryProvider> m_inventory_provider;
    std::shared_ptr<domain::interfaces::ISmartProvider> m_smart_provider;
    domain::models::StorageStats m_stats;
    QVariantList m_drives;
    QTimer *m_timer;
    bool m_busy = false;
    bool m_inventory_busy = false;
};

} // namespace my_comp::presentation::viewmodels
