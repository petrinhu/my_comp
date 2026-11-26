#include "application/vram/configuracao/sublocators/lista_wrappers_desktop.h"

namespace my_comp::application::vram {

const QHash<QString, domain::vram::Confianca>& lista_wrappers_desktop() {
    static const QHash<QString, domain::vram::Confianca> tabela = {
        {QStringLiteral("prime-run"),   domain::vram::Confianca::Alta},
        {QStringLiteral("optirun"),     domain::vram::Confianca::Alta},
        {QStringLiteral("primusrun"),   domain::vram::Confianca::Alta},
        {QStringLiteral("nvidia-run"),  domain::vram::Confianca::Alta},
        {QStringLiteral("vk_pro"),      domain::vram::Confianca::Alta},
        {QStringLiteral("gamemoderun"), domain::vram::Confianca::Media},
    };
    return tabela;
}

}
