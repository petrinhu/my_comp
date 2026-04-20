#pragma once
#include <QObject>
#include <QVariantList>
#include <atomic>
#include <memory>
#include "../../domain/interfaces/i_directory_scanner.h"

namespace my_comp::presentation::viewmodels {

class DirectoryScannerViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY isScanningChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY progressChanged)
    Q_PROPERTY(qlonglong bytesScanned READ bytesScanned NOTIFY progressChanged)
    Q_PROPERTY(int filesScanned READ filesScanned NOTIFY progressChanged)
    Q_PROPERTY(QString scannedRoot READ scannedRoot NOTIFY resultChanged)
    Q_PROPERTY(qlonglong totalSize READ totalSize NOTIFY resultChanged)
    Q_PROPERTY(QVariantList topEntries READ topEntries NOTIFY resultChanged)

public:
    explicit DirectoryScannerViewModel(std::shared_ptr<domain::interfaces::IDirectoryScanner> scanner,
                                       QObject* parent = nullptr);

    bool isScanning() const { return m_is_scanning; }
    QString currentPath() const { return m_current_path; }
    qlonglong bytesScanned() const { return m_bytes_scanned; }
    int filesScanned() const { return m_files_scanned; }
    QString scannedRoot() const { return m_scanned_root; }
    qlonglong totalSize() const { return m_total_size; }
    QVariantList topEntries() const { return m_top_entries; }

    Q_INVOKABLE void scan(const QString& path);
    Q_INVOKABLE void cancel();

signals:
    void isScanningChanged();
    void progressChanged();
    void resultChanged();

private:
    std::shared_ptr<domain::interfaces::IDirectoryScanner> m_scanner;
    std::shared_ptr<std::atomic<bool>> m_cancel_token;
    bool m_is_scanning = false;
    QString m_current_path;
    qlonglong m_bytes_scanned = 0;
    int m_files_scanned = 0;
    QString m_scanned_root;
    qlonglong m_total_size = 0;
    QVariantList m_top_entries;
};

} // namespace my_comp::presentation::viewmodels
