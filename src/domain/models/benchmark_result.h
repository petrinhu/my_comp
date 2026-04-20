#pragma once
#include <QString>

namespace my_comp::domain::models {

struct BenchmarkResult {
    QString device;
    double seq_read_mb_s = 0.0;    // Timing buffered disk reads
    double cached_mb_s = 0.0;      // Timing cached reads
    double duration_s = 0.0;
    bool success = false;
    QString error_message;
};

} // namespace my_comp::domain::models
