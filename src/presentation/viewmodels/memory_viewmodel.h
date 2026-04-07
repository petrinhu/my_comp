#pragma once
#include <QObject>
#include <QTimer>
#include <memory>
#include "../../domain/interfaces/i_memory_provider.h"

namespace my_comp::presentation::viewmodels {

class MemoryViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString ramTotal READ ramTotal NOTIFY statsChanged)
    Q_PROPERTY(QString ramUsed READ ramUsed NOTIFY statsChanged)
    Q_PROPERTY(double ramUsagePercent READ ramUsagePercent NOTIFY statsChanged)
    
    Q_PROPERTY(QString swapTotal READ swapTotal NOTIFY statsChanged)
    Q_PROPERTY(QString swapUsed READ swapUsed NOTIFY statsChanged)
    Q_PROPERTY(double swapUsagePercent READ swapUsagePercent NOTIFY statsChanged)
    
    Q_PROPERTY(bool hasZram READ hasZram NOTIFY statsChanged)
    Q_PROPERTY(double zramCompressionRatio READ zramCompressionRatio NOTIFY statsChanged)

public:
    explicit MemoryViewModel(std::shared_ptr<domain::interfaces::IMemoryProvider> provider, QObject *parent = nullptr);

    QString ramTotal() const { return format_kb(m_stats.ram_total_kb); }
    QString ramUsed() const { return format_kb(m_stats.ram_used_kb); }
    double ramUsagePercent() const { return m_stats.ram_usage_percent; }

    QString swapTotal() const { return format_kb(m_stats.swap_total_kb); }
    QString swapUsed() const { return format_kb(m_stats.swap_used_kb); }
    double swapUsagePercent() const { return m_stats.swap_usage_percent; }

    bool hasZram() const { return m_stats.has_zram; }
    double zramCompressionRatio() const { return m_stats.zram_compression_ratio; }

public slots:
    void refresh();

signals:
    void statsChanged();

private:
    QString format_kb(uint64_t kb) const;
    
    std::shared_ptr<domain::interfaces::IMemoryProvider> m_provider;
    domain::models::MemoryStats m_stats;
    QTimer *m_timer;
};

} // namespace my_comp::presentation::viewmodels
