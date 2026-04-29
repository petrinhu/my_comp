#include "mid/configuracao/sublocators/lista_wrappers_desktop.h"

namespace vramopt::application {

const QHash<QString, domain::Confianca>& listaWrappersDesktop() {
    static const QHash<QString, domain::Confianca> tabela = {
        {QStringLiteral("prime-run"),   domain::Confianca::Alta},
        {QStringLiteral("optirun"),     domain::Confianca::Alta},
        {QStringLiteral("primusrun"),   domain::Confianca::Alta},
        {QStringLiteral("nvidia-run"),  domain::Confianca::Alta},
        {QStringLiteral("vk_pro"),      domain::Confianca::Alta},
        {QStringLiteral("gamemoderun"), domain::Confianca::Media},
    };
    return tabela;
}

}
