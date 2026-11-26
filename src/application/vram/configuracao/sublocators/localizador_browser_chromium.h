#pragma once

#include "domain/vram/interfaces/i_config_locator.h"
#include "domain/vram/interfaces/i_leitor_arquivo_config.h"

#include <QString>

#include <memory>

namespace my_comp::application::vram {

class LocalizadorBrowserChromium final : public domain::vram::IConfigLocator {
public:
    // home_dir vem da composição (MC-02, AUDIT-34): a aplicação não consulta o SO.
    LocalizadorBrowserChromium(std::shared_ptr<domain::vram::ILeitorArquivoConfig> leitor, QString home_dir);

    [[nodiscard]] QVector<domain::vram::OrigemConfig>
        localizar(const domain::vram::Processo& processo) const override;

private:
    std::shared_ptr<domain::vram::ILeitorArquivoConfig> m_leitor;
    QString m_home_dir;
};

}
