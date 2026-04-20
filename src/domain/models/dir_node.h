#pragma once
#include <QString>
#include <vector>

namespace my_comp::domain::models {

struct DirNode {
    QString path;
    QString name;
    qint64 size_bytes = 0;   // total recursivo
    int file_count = 0;      // arquivos regulares (recursivo)
    int dir_count = 0;       // subdiretórios (recursivo)
    bool is_dir = true;
    bool error = false;      // true = sem permissão / link quebrado / etc.
    std::vector<DirNode> children;
};

struct ScanProgress {
    QString current_path;
    qint64 bytes_scanned = 0;
    int files_scanned = 0;
    int dirs_scanned = 0;
};

} // namespace my_comp::domain::models
