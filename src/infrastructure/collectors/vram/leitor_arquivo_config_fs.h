#pragma once

#include "domain/vram/interfaces/i_leitor_arquivo_config.h"

namespace vramopt::infrastructure {

class LeitorArquivoConfigFs final : public domain::ILeitorArquivoConfig {
public:
    [[nodiscard]] std::optional<QByteArray>
        ler(const QString& path) const override;
};

}
