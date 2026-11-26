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

    FnInit                  fn_init{nullptr};
    FnShutdown              fn_shutdown{nullptr};
    FnDeviceGetCount        fn_get_count{nullptr};
    FnDeviceGetHandleByIdx  fn_get_handle{nullptr};
    FnDeviceGetName         fn_get_name{nullptr};
    FnDeviceGetMemoryInfo   fn_get_mem{nullptr};
    FnDeviceGetCompProcs    fn_get_comp_procs{nullptr};
    FnDeviceGetCompProcs    fn_get_graphics_procs{nullptr}; // mesma assinatura da variante compute

    template <typename T>
    static T resolve(void* h, const char* nome_v2_pref, const char* nome_legacy) {
        void* p = dlsym(h, nome_v2_pref);
        if (p == nullptr && nome_legacy != nullptr) {
            p = dlsym(h, nome_legacy);
        }
        return reinterpret_cast<T>(p);
    }

    [[nodiscard]] bool simbolos_ok() const {
        return fn_init != nullptr && fn_shutdown != nullptr
               && fn_get_count != nullptr && fn_get_handle != nullptr
               && fn_get_name != nullptr && fn_get_mem != nullptr
               && fn_get_comp_procs != nullptr;
    }
};

NvmlAdapterDl::NvmlAdapterDl()
    : m_impl{std::make_unique<Impl>()} {
    m_impl->handle = dlopen("libnvidia-ml.so.1", RTLD_NOW | RTLD_LOCAL);
    if (m_impl->handle == nullptr) {
        return;
    }
    m_impl->fn_init       = Impl::resolve<FnInit>(m_impl->handle,
                                                 "nvmlInit_v2", "nvmlInit");
    m_impl->fn_shutdown   = Impl::resolve<FnShutdown>(m_impl->handle,
                                                     "nvmlShutdown", nullptr);
    m_impl->fn_get_count   = Impl::resolve<FnDeviceGetCount>(m_impl->handle,
                                                           "nvmlDeviceGetCount_v2",
                                                           "nvmlDeviceGetCount");
    m_impl->fn_get_handle  = Impl::resolve<FnDeviceGetHandleByIdx>(m_impl->handle,
                                                                 "nvmlDeviceGetHandleByIndex_v2",
                                                                 "nvmlDeviceGetHandleByIndex");
    m_impl->fn_get_name    = Impl::resolve<FnDeviceGetName>(m_impl->handle,
                                                          "nvmlDeviceGetName", nullptr);
    m_impl->fn_get_mem     = Impl::resolve<FnDeviceGetMemoryInfo>(m_impl->handle,
                                                                "nvmlDeviceGetMemoryInfo", nullptr);
    m_impl->fn_get_comp_procs = Impl::resolve<FnDeviceGetCompProcs>(m_impl->handle,
                                                                 "nvmlDeviceGetComputeRunningProcesses_v3",
                                                                 "nvmlDeviceGetComputeRunningProcesses_v2");

    // Opcional: drivers antigos podem não exportar; a lista graphics fica vazia.
    m_impl->fn_get_graphics_procs = Impl::resolve<FnDeviceGetCompProcs>(m_impl->handle,
                                                                     "nvmlDeviceGetGraphicsRunningProcesses_v3",
                                                                     "nvmlDeviceGetGraphicsRunningProcesses_v2");

    if (!m_impl->simbolos_ok()) {
        return;
    }
    if (m_impl->fn_init() == NVML_SUCCESS) {
        m_impl->inited = true;
    }
}

NvmlAdapterDl::~NvmlAdapterDl() {
    if (m_impl && m_impl->inited && m_impl->fn_shutdown != nullptr) {
        (void)m_impl->fn_shutdown();
    }
    if (m_impl && m_impl->handle != nullptr) {
        (void)dlclose(m_impl->handle);
    }
}

bool NvmlAdapterDl::is_available() const {
    return m_impl && m_impl->inited;
}

std::uint32_t NvmlAdapterDl::device_count() {
    if (!is_available()) {
        return 0;
    }
    unsigned int count = 0;
    if (m_impl->fn_get_count(&count) != NVML_SUCCESS) {
        return 0;
    }
    return count;
}

std::optional<NvmlDeviceInfo> NvmlAdapterDl::device_info(std::uint32_t idx) {
    if (!is_available()) {
        return std::nullopt;
    }
    NvmlDevice_t dev = nullptr;
    if (m_impl->fn_get_handle(idx, &dev) != NVML_SUCCESS || dev == nullptr) {
        return std::nullopt;
    }
    char nome[NVML_DEVICE_NAME_BUFFER_SIZE]{};
    if (m_impl->fn_get_name(dev, nome, NVML_DEVICE_NAME_BUFFER_SIZE) != NVML_SUCCESS) {
        return std::nullopt;
    }
    NvmlMemory mem{};
    if (m_impl->fn_get_mem(dev, &mem) != NVML_SUCCESS) {
        return std::nullopt;
    }
    NvmlDeviceInfo out;
    out.nome = QString::fromUtf8(nome, static_cast<int>(strnlen(nome, NVML_DEVICE_NAME_BUFFER_SIZE)));
    out.vram_total_bytes = mem.total;
    return out;
}

namespace {

// Protocolo de duas chamadas do NVML: 1ª com buffer nulo devolve o tamanho
// (INSUFFICIENT_SIZE); 2ª preenche. Se a lista crescer entre as duas, desiste do ciclo.
template <typename Fn>
std::vector<NvmlProcessoInfo> listar_processos(Fn fn, NvmlDevice_t dev) {
    std::vector<NvmlProcessoInfo> resultado;
    if (fn == nullptr) {
        return resultado;
    }
    unsigned int n = 0;
    auto rc = fn(dev, &n, nullptr);
    if (n == 0 && rc == NVML_SUCCESS) {
        return resultado;
    }
    if (rc != NVML_ERROR_INSUFFICIENT_SIZE && rc != NVML_SUCCESS) {
        return resultado;
    }
    std::vector<NvmlProcessInfo_v2> buffer(n);
    rc = fn(dev, &n, buffer.data());
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

}

std::vector<NvmlProcessoInfo> NvmlAdapterDl::processos_compute(std::uint32_t idx) {
    if (!is_available()) {
        return {};
    }
    NvmlDevice_t dev = nullptr;
    if (m_impl->fn_get_handle(idx, &dev) != NVML_SUCCESS || dev == nullptr) {
        return {};
    }
    return listar_processos(m_impl->fn_get_comp_procs, dev);
}

std::vector<NvmlProcessoInfo> NvmlAdapterDl::processos_graphics(std::uint32_t idx) {
    if (!is_available()) {
        return {};
    }
    NvmlDevice_t dev = nullptr;
    if (m_impl->fn_get_handle(idx, &dev) != NVML_SUCCESS || dev == nullptr) {
        return {};
    }
    return listar_processos(m_impl->fn_get_graphics_procs, dev);
}

std::shared_ptr<INvmlAdapter> criar_adapter_nvml_real() {
    return std::make_shared<NvmlAdapterDl>();
}

}
