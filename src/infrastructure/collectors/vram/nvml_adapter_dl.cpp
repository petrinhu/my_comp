#include "infrastructure/collectors/vram/nvml_adapter_dl.h"

#include <QString>

#include <cstring>
#include <dlfcn.h>

namespace vramopt::infrastructure {

namespace {

// Subconjunto mínimo da API NVML 11+ redeclarado para permitir dlopen sem
// depender do header oficial (que requer pacote cuda-toolkit-dev).
//
// Referência: NVML Reference Manual r570, seção "Device Queries" + "Process".

using NvmlReturn = int;
constexpr NvmlReturn NVML_SUCCESS                  = 0;
constexpr NvmlReturn NVML_ERROR_INSUFFICIENT_SIZE  = 7;

using NvmlDevice_t = void*;

struct NvmlMemory {
    unsigned long long total;
    unsigned long long free;
    unsigned long long used;
};

// Layout do nvmlProcessInfo_v2 do NVML 11+. Tamanho: 24 bytes em x86_64.
struct NvmlProcessInfo_v2 {
    unsigned int       pid;
    unsigned long long usedGpuMemory;
    unsigned int       gpuInstanceId;
    unsigned int       computeInstanceId;
};

constexpr unsigned int NVML_DEVICE_NAME_BUFFER_SIZE = 64U;

using FnInit                  = NvmlReturn (*)();
using FnShutdown              = NvmlReturn (*)();
using FnDeviceGetCount        = NvmlReturn (*)(unsigned int*);
using FnDeviceGetHandleByIdx  = NvmlReturn (*)(unsigned int, NvmlDevice_t*);
using FnDeviceGetName         = NvmlReturn (*)(NvmlDevice_t, char*, unsigned int);
using FnDeviceGetMemoryInfo   = NvmlReturn (*)(NvmlDevice_t, NvmlMemory*);
using FnDeviceGetCompProcs    = NvmlReturn (*)(NvmlDevice_t, unsigned int*,
                                               NvmlProcessInfo_v2*);

}

struct NvmlAdapterDl::Impl {
    void* handle{nullptr};
    bool  inited{false};

    FnInit                  fnInit{nullptr};
    FnShutdown              fnShutdown{nullptr};
    FnDeviceGetCount        fnGetCount{nullptr};
    FnDeviceGetHandleByIdx  fnGetHandle{nullptr};
    FnDeviceGetName         fnGetName{nullptr};
    FnDeviceGetMemoryInfo   fnGetMem{nullptr};
    FnDeviceGetCompProcs    fnGetCompProcs{nullptr};

    template <typename T>
    static T resolve(void* h, const char* nome_v2_pref, const char* nome_legacy) {
        void* p = dlsym(h, nome_v2_pref);
        if (p == nullptr && nome_legacy != nullptr) {
            p = dlsym(h, nome_legacy);
        }
        return reinterpret_cast<T>(p);
    }

    [[nodiscard]] bool simbolosOk() const {
        return fnInit != nullptr && fnShutdown != nullptr
               && fnGetCount != nullptr && fnGetHandle != nullptr
               && fnGetName != nullptr && fnGetMem != nullptr
               && fnGetCompProcs != nullptr;
    }
};

NvmlAdapterDl::NvmlAdapterDl()
    : m_impl{std::make_unique<Impl>()} {
    m_impl->handle = dlopen("libnvidia-ml.so.1", RTLD_NOW | RTLD_LOCAL);
    if (m_impl->handle == nullptr) {
        return;
    }
    m_impl->fnInit       = Impl::resolve<FnInit>(m_impl->handle,
                                                 "nvmlInit_v2", "nvmlInit");
    m_impl->fnShutdown   = Impl::resolve<FnShutdown>(m_impl->handle,
                                                     "nvmlShutdown", nullptr);
    m_impl->fnGetCount   = Impl::resolve<FnDeviceGetCount>(m_impl->handle,
                                                           "nvmlDeviceGetCount_v2",
                                                           "nvmlDeviceGetCount");
    m_impl->fnGetHandle  = Impl::resolve<FnDeviceGetHandleByIdx>(m_impl->handle,
                                                                 "nvmlDeviceGetHandleByIndex_v2",
                                                                 "nvmlDeviceGetHandleByIndex");
    m_impl->fnGetName    = Impl::resolve<FnDeviceGetName>(m_impl->handle,
                                                          "nvmlDeviceGetName", nullptr);
    m_impl->fnGetMem     = Impl::resolve<FnDeviceGetMemoryInfo>(m_impl->handle,
                                                                "nvmlDeviceGetMemoryInfo", nullptr);
    m_impl->fnGetCompProcs = Impl::resolve<FnDeviceGetCompProcs>(m_impl->handle,
                                                                 "nvmlDeviceGetComputeRunningProcesses_v3",
                                                                 "nvmlDeviceGetComputeRunningProcesses_v2");

    if (!m_impl->simbolosOk()) {
        return;
    }
    if (m_impl->fnInit() == NVML_SUCCESS) {
        m_impl->inited = true;
    }
}

NvmlAdapterDl::~NvmlAdapterDl() {
    if (m_impl && m_impl->inited && m_impl->fnShutdown != nullptr) {
        (void)m_impl->fnShutdown();
    }
    if (m_impl && m_impl->handle != nullptr) {
        (void)dlclose(m_impl->handle);
    }
}

bool NvmlAdapterDl::isAvailable() const {
    return m_impl && m_impl->inited;
}

std::uint32_t NvmlAdapterDl::deviceCount() {
    if (!isAvailable()) {
        return 0;
    }
    unsigned int count = 0;
    if (m_impl->fnGetCount(&count) != NVML_SUCCESS) {
        return 0;
    }
    return count;
}

std::optional<NvmlDeviceInfo> NvmlAdapterDl::deviceInfo(std::uint32_t idx) {
    if (!isAvailable()) {
        return std::nullopt;
    }
    NvmlDevice_t dev = nullptr;
    if (m_impl->fnGetHandle(idx, &dev) != NVML_SUCCESS || dev == nullptr) {
        return std::nullopt;
    }
    char nome[NVML_DEVICE_NAME_BUFFER_SIZE]{};
    if (m_impl->fnGetName(dev, nome, NVML_DEVICE_NAME_BUFFER_SIZE) != NVML_SUCCESS) {
        return std::nullopt;
    }
    NvmlMemory mem{};
    if (m_impl->fnGetMem(dev, &mem) != NVML_SUCCESS) {
        return std::nullopt;
    }
    NvmlDeviceInfo out;
    out.nome = QString::fromUtf8(nome, static_cast<int>(strnlen(nome, NVML_DEVICE_NAME_BUFFER_SIZE)));
    out.vram_total_bytes = mem.total;
    return out;
}

std::vector<NvmlProcessoInfo> NvmlAdapterDl::processosCompute(std::uint32_t idx) {
    std::vector<NvmlProcessoInfo> resultado;
    if (!isAvailable()) {
        return resultado;
    }
    NvmlDevice_t dev = nullptr;
    if (m_impl->fnGetHandle(idx, &dev) != NVML_SUCCESS || dev == nullptr) {
        return resultado;
    }
    unsigned int n = 0;
    auto rc = m_impl->fnGetCompProcs(dev, &n, nullptr);
    if (n == 0 && rc == NVML_SUCCESS) {
        return resultado;
    }
    if (rc != NVML_ERROR_INSUFFICIENT_SIZE && rc != NVML_SUCCESS) {
        return resultado;
    }
    std::vector<NvmlProcessInfo_v2> buffer(n);
    rc = m_impl->fnGetCompProcs(dev, &n, buffer.data());
    if (rc != NVML_SUCCESS) {
        return resultado;
    }
    resultado.reserve(n);
    for (unsigned int i = 0; i < n; ++i) {
        resultado.push_back(NvmlProcessoInfo{
            .pid = buffer[i].pid,
            .bytes = buffer[i].usedGpuMemory,
        });
    }
    return resultado;
}

std::shared_ptr<INvmlAdapter> criarAdapterNvmlReal() {
    return std::make_shared<NvmlAdapterDl>();
}

}
