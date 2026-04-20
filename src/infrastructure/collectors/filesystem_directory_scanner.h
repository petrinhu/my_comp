#pragma once
#include "../../domain/interfaces/i_directory_scanner.h"

namespace my_comp::infrastructure::collectors {

class FilesystemDirectoryScanner : public domain::interfaces::IDirectoryScanner {
public:
    FilesystemDirectoryScanner() = default;

    domain::models::DirNode scan(const QString& root,
                                 std::shared_ptr<std::atomic<bool>> cancel_token,
                                 ProgressCallback progress_cb) override;
};

} // namespace my_comp::infrastructure::collectors
