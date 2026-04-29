// BT-02 — libFuzzer harness para LinuxDevStackProvider::parse_containers_json.
// Cobre: array JSON (podman) E NDJSON (docker). Run: ./build/fuzz_containers_parser corpus/

#include <cstdint>
#include <cstddef>
#include <vector>
#include <QString>
#include "../../src/infrastructure/collectors/linux_dev_stack_provider.h"

using my_comp::infrastructure::collectors::LinuxDevStackProvider;
using my_comp::domain::models::ContainerInfo;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > 65536) return 0;
    const QString input = QString::fromUtf8(reinterpret_cast<const char*>(data), static_cast<int>(size));
    std::vector<ContainerInfo> out;
    // O 1º byte alterna entre rotular como Podman/Docker — exercita ambos os ramos do parser.
    const QString runtime = (size > 0 && (data[0] & 1)) ? QStringLiteral("Podman") : QStringLiteral("Docker");
    LinuxDevStackProvider::parse_containers_json(input, runtime, out);
    return 0;
}
