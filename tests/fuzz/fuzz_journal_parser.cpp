// BT-02 — libFuzzer harness para JournalDiagnosticProvider::parse_journalctl_output.
// Build: cmake -B build -DMY_COMP_BUILD_FUZZERS=ON -DCMAKE_CXX_COMPILER=clang++.
// Run:   ./build/fuzz_journal_parser corpus/ -max_total_time=60 -timeout=5

#include <cstdint>
#include <cstddef>
#include <vector>
#include <QString>
#include "../../src/infrastructure/collectors/journal_diagnostic_provider.h"

using my_comp::infrastructure::collectors::JournalDiagnosticProvider;
using my_comp::domain::models::LogEntry;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > 65536) return 0; // Cap: bench, não stress de I/O.
    const QString input = QString::fromUtf8(reinterpret_cast<const char*>(data), static_cast<int>(size));
    std::vector<LogEntry> out;
    JournalDiagnosticProvider::parse_journalctl_output(input, out);
    return 0;
}
