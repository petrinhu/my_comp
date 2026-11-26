#include "directory_scanner_viewmodel.h"
#include <QPointer>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>
#include <algorithm>

namespace my_comp::presentation::viewmodels {

DirectoryScannerViewModel::DirectoryScannerViewModel(
    std::shared_ptr<domain::interfaces::IDirectoryScanner> scanner, QObject* parent)
    : QObject(parent), m_scanner(std::move(scanner)) {}

void DirectoryScannerViewModel::scan(const QString& path) {
    if (m_is_scanning || !m_scanner) return;

    m_cancel_token = std::make_shared<std::atomic<bool>>(false);
    m_is_scanning = true;
    m_current_path.clear();
    m_bytes_scanned = 0;
    m_files_scanned = 0;
    m_scanned_root = path;
    m_total_size = 0;
    m_top_entries.clear();
    emit isScanningChanged();
    emit progressChanged();
    emit resultChanged();

    const QPointer<DirectoryScannerViewModel> self(this);
    auto scanner = m_scanner;
    auto token = m_cancel_token;

    auto progress_cb = [self](const domain::models::ScanProgress& p) {
        QMetaObject::invokeMethod(self.data(), [self, p]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_current_path = p.current_path;
            vm->m_bytes_scanned = p.bytes_scanned;
            vm->m_files_scanned = p.files_scanned;
            emit vm->progressChanged();
        }, Qt::QueuedConnection);
    };

    (void)QtConcurrent::run([self, scanner, token, path, progress_cb]() {
        auto root = scanner->scan(path, token, progress_cb);

        // Coletar os 20 maiores filhos imediatos do root (critério pragmático até ST-06 definir treemap).
        std::vector<domain::models::DirNode> entries = root.children;
        std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
            return a.size_bytes > b.size_bytes;
        });
        if (entries.size() > 20) entries.resize(20);

        QVariantList top;
        for (const auto& n : entries) {
            QVariantMap m;
            m["name"] = n.name;
            m["path"] = n.path;
            m["sizeBytes"] = static_cast<qlonglong>(n.size_bytes);
            m["fileCount"] = n.file_count;
            m["dirCount"] = n.dir_count;
            m["error"] = n.error;
            top.append(m);
        }
        const qlonglong total = static_cast<qlonglong>(root.size_bytes);

        QMetaObject::invokeMethod(self.data(), [self, top, total]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_top_entries = top;
            vm->m_total_size = total;
            vm->m_is_scanning = false;
            emit vm->resultChanged();
            emit vm->isScanningChanged();
        }, Qt::QueuedConnection);
    });
}

void DirectoryScannerViewModel::cancel() {
    if (m_cancel_token) m_cancel_token->store(true);
}

} // namespace my_comp::presentation::viewmodels
