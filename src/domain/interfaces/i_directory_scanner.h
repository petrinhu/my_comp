#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include "../models/dir_node.h"

namespace my_comp::domain::interfaces {

class IDirectoryScanner {
public:
    using ProgressCallback = std::function<void(const models::ScanProgress&)>;

    virtual ~IDirectoryScanner() = default;

    // `cancel_token` é compartilhado com a UI — setter externo pode interromper a varredura.
    // `progress_cb` pode ser nullptr; a implementação deve throttle (ex.: a cada N arquivos).
    virtual models::DirNode scan(const QString& root,
                                 std::shared_ptr<std::atomic<bool>> cancel_token,
                                 ProgressCallback progress_cb) = 0;
};

} // namespace my_comp::domain::interfaces
