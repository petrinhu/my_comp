#include "application/vram/classificacao/regras/lista_libs_compute.h"

namespace vramopt::application {

ListaLibsCompute::ListaLibsCompute(QStringList padroes_compute)
    : m_padroes_compute{std::move(padroes_compute)} {}

ListaLibsCompute ListaLibsCompute::padrao() {
    return ListaLibsCompute{QStringList{
        QStringLiteral("libcuda.so"),
        QStringLiteral("libcudart.so"),
        QStringLiteral("libcudnn.so"),
        QStringLiteral("libnvinfer.so"),
        QStringLiteral("libnvrtc.so"),
        QStringLiteral("libze_loader.so"),
        QStringLiteral("libze_intel_gpu.so"),
        QStringLiteral("libOpenCL.so"),
        QStringLiteral("libamdhip64.so"),
        QStringLiteral("libhsa-runtime64.so"),
        QStringLiteral("librocm_smi64.so"),
    }};
}

std::optional<QString>
ListaLibsCompute::primeiraQueCasa(const QStringList& bibliotecas_carregadas) const {
    for (const auto& padrao : m_padroes_compute) {
        for (const auto& lib : bibliotecas_carregadas) {
            if (lib.contains(padrao)) {
                return padrao;
            }
        }
    }
    return std::nullopt;
}

}
