#include "infrastructure/collectors/vram/factory_gpu_backend.h"
#include "infrastructure/collectors/vram/intel_i915_gpu_backend.h"
#include "infrastructure/collectors/vram/nvml_adapter.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QTemporaryDir>

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <vector>

using my_comp::infrastructure::vram::criar_backend_composito_com;
using my_comp::infrastructure::vram::DepsFactoryGpuBackend;
using my_comp::infrastructure::vram::INvmlAdapter;
using my_comp::infrastructure::vram::NvmlDeviceInfo;
using my_comp::infrastructure::vram::NvmlProcessoInfo;

namespace {

class StubNvmlAdapter : public INvmlAdapter {
public:
    bool                        avail = true;
    std::vector<NvmlDeviceInfo> devices;
    [[nodiscard]] bool is_available() const override { return avail; }
    [[nodiscard]] std::uint32_t device_count() override {
        return static_cast<std::uint32_t>(devices.size());
    }
    [[nodiscard]] std::optional<NvmlDeviceInfo> device_info(std::uint32_t idx) override {
        if (idx >= devices.size()) {
            return std::nullopt;
        }
        return devices[idx];
    }
    [[nodiscard]] std::vector<NvmlProcessoInfo>
        processos_compute(std::uint32_t /*idx*/) override { return {}; }
};

}

TEST(FactoryGpuBackendTest, RetornaNullBackendQuandoNvmlIndisponivelESemIntel) {
    DepsFactoryGpuBackend deps;
    deps.criar_adapter_nvml = []() {
        auto a = std::make_shared<StubNvmlAdapter>();
        a->avail = false;
        return a;
    };
    deps.sistema_tem_intel = [](const QString&) { return false; };

    auto backend = criar_backend_composito_com(deps);
    ASSERT_NE(backend, nullptr);
    EXPECT_TRUE(backend->enumerar_gpus().empty());
}

TEST(FactoryGpuBackendTest, AgregaApenasNvmlQuandoIntelAusente) {
    DepsFactoryGpuBackend deps;
    deps.criar_adapter_nvml = []() {
        auto a = std::make_shared<StubNvmlAdapter>();
        a->avail = true;
        a->devices = {NvmlDeviceInfo{
            .nome = QStringLiteral("NVIDIA RTX 4090"),
            .vram_total_bytes = 24ULL * 1024 * 1024 * 1024,
        }};
        return a;
    };
    deps.sistema_tem_intel = [](const QString&) { return false; };

    auto backend = criar_backend_composito_com(deps);
    const auto gpus = backend->enumerar_gpus();
    ASSERT_EQ(gpus.size(), 1U);
    EXPECT_EQ(gpus[0].fabricante(), my_comp::domain::vram::Fabricante::Nvidia);
}

TEST(FactoryGpuBackendTest, AgregaApenasIntelQuandoNvmlIndisponivel) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString drm = tmp.path() + QStringLiteral("/drm");
    QDir{}.mkpath(drm + QStringLiteral("/card0/device"));
    QFile vendor{drm + QStringLiteral("/card0/device/vendor")};
    ASSERT_TRUE(vendor.open(QIODevice::WriteOnly | QIODevice::Text));
    vendor.write("0x8086\n");
    vendor.close();
    QFile uev{drm + QStringLiteral("/card0/device/uevent")};
    ASSERT_TRUE(uev.open(QIODevice::WriteOnly | QIODevice::Text));
    uev.write("DRIVER=i915\nPCI_ID=8086:9A49\nPCI_SLOT_NAME=0000:00:02.0\n");
    uev.close();

    DepsFactoryGpuBackend deps;
    deps.criar_adapter_nvml = []() {
        auto a = std::make_shared<StubNvmlAdapter>();
        a->avail = false;
        return a;
    };
    deps.sistema_tem_intel = [](const QString& d) {
        return my_comp::infrastructure::vram::IntelI915GpuBackend::sistema_tem_gpu_intel(d);
    };
    deps.drm_root  = drm;
    deps.proc_root = tmp.path() + QStringLiteral("/proc");

    auto backend = criar_backend_composito_com(deps);
    const auto gpus = backend->enumerar_gpus();
    ASSERT_EQ(gpus.size(), 1U);
    EXPECT_EQ(gpus[0].fabricante(), my_comp::domain::vram::Fabricante::Intel);
}
