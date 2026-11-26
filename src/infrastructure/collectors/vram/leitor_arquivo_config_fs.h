#pragma once

#include "domain/vram/interfaces/i_leitor_arquivo_config.h"

namespace my_comp::infrastructure::vram {

class LeitorArquivoConfigFs final : public domain::vram::ILeitorArquivoConfig {
public:
    [[nodiscard]] std::optional<QByteArray>
        ler(const QString& path) const override;
};

}
