#pragma once
#include <QString>
#include <vector>

namespace my_comp::domain::models {

struct SmartAttribute {
    int id = 0;
    QString name;
    qint64 raw_value = 0;
    int threshold = 0;
};

struct SmartReport {
    bool available = false;       // false = smartctl ausente, sem permissão, ou device não suporta
    bool passed = false;          // smart_status.passed
    int temp_c = 0;
    qint64 power_on_hours = 0;
    int wear_pct = 0;             // 0-100 (100 = fim de vida). NVMe: percentage_used.
    QString error_message;        // motivo da indisponibilidade (permission denied, etc.)
    std::vector<SmartAttribute> attrs;
};

} // namespace my_comp::domain::models
