#include "filesystem_directory_scanner.h"
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <chrono>

namespace my_comp::infrastructure::collectors {

namespace {

using namespace my_comp::domain::models;

// Pseudo-filesystems e locais voláteis: varredura quase sempre quebra (ciclos, devices, /proc absurdo).
const QSet<QString> kSkipRoots{
    "/proc", "/sys", "/run", "/dev",
    "/var/run", "/var/lock",
    "/snap", "/media"  // snap: mountpoints efêmeros; media: mounts removíveis que podem desaparecer
};

struct Throttle {
    std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
    bool due(int ms) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count() >= ms) {
            last = now;
            return true;
        }
        return false;
    }
};

DirNode scan_recursive(const QString& path,
                       std::shared_ptr<std::atomic<bool>> cancel_token,
                       const my_comp::domain::interfaces::IDirectoryScanner::ProgressCallback& cb,
                       ScanProgress& progress,
                       Throttle& throttle) {
    DirNode node;
    node.path = path;
    node.name = QFileInfo(path).fileName();
    if (node.name.isEmpty()) node.name = path; // root "/"

    if (cancel_token && cancel_token->load()) return node;
    if (kSkipRoots.contains(path)) { node.error = true; return node; }

    QDir dir(path);
    if (!dir.exists()) { node.error = true; return node; }

    // Não seguir symlinks (QDir::NoSymLinks) para evitar ciclos e evasão fora do root.
    const auto entries = dir.entryInfoList(
        QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System,
        QDir::NoSort);

    for (const auto& fi : entries) {
        if (cancel_token && cancel_token->load()) return node;
        if (fi.isSymLink()) continue;

        if (fi.isDir()) {
            auto child = scan_recursive(fi.absoluteFilePath(), cancel_token, cb, progress, throttle);
            node.size_bytes += child.size_bytes;
            node.file_count += child.file_count;
            node.dir_count += child.dir_count + 1;
            node.children.push_back(std::move(child));
        } else if (fi.isFile()) {
            const qint64 sz = fi.size();
            node.size_bytes += sz;
            node.file_count += 1;
            progress.bytes_scanned += sz;
            progress.files_scanned += 1;
        }

        if (cb && throttle.due(100)) {
            progress.current_path = path;
            cb(progress);
        }
    }
    return node;
}

} // namespace

domain::models::DirNode FilesystemDirectoryScanner::scan(
    const QString& root,
    std::shared_ptr<std::atomic<bool>> cancel_token,
    ProgressCallback progress_cb) {

    ScanProgress progress;
    Throttle throttle;
    auto result = scan_recursive(root, std::move(cancel_token), progress_cb, progress, throttle);
    // emite snapshot final
    if (progress_cb) {
        progress.current_path = root;
        progress_cb(progress);
    }
    return result;
}

} // namespace my_comp::infrastructure::collectors
