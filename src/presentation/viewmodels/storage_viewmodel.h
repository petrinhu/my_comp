#pragma once
#include <QObject>
#include <QTimer>
#include <memory>
#include "../../domain/interfaces/i_storage_provider.h"

namespace my_comp::presentation::viewmodels {

class StorageViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(double totalReadMb READ totalReadMb NOTIFY statsChanged)
    Q_PROPERTY(double totalWriteMb READ totalWriteMb NOTIFY statsChanged)
    Q_PROPERTY(QString totalThroughput READ totalThroughput NOTIFY statsChanged)

public:
    explicit StorageViewModel(std::shared_ptr<domain::interfaces::IStorageProvider> provider, QObject *parent = nullptr);

    double totalReadMb() const { return m_stats.total_read_mb_s; }
    double totalWriteMb() const { return m_stats.total_write_mb_s; }
    QString totalThroughput() const;

public slots:
    void refresh();

signals:
    void statsChanged();

private:
    std::shared_ptr<domain::interfaces::IStorageProvider> m_provider;
    domain::models::StorageStats m_stats;
    QTimer *m_timer;
};

} // namespace my_comp::presentation::viewmodels
