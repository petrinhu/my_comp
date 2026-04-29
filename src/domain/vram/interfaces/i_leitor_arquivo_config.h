#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace vramopt::domain {

class ILeitorArquivoConfig {
public:
    ILeitorArquivoConfig() = default;
    ILeitorArquivoConfig(const ILeitorArquivoConfig&) = delete;
    ILeitorArquivoConfig(ILeitorArquivoConfig&&) = delete;
    ILeitorArquivoConfig& operator=(const ILeitorArquivoConfig&) = delete;
    ILeitorArquivoConfig& operator=(ILeitorArquivoConfig&&) = delete;
    virtual ~ILeitorArquivoConfig() = default;

    [[nodiscard]] virtual std::optional<QByteArray>
        ler(const QString& path) const = 0;
};

}
