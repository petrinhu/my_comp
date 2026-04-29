#pragma once

#include "back/configuracao/origem_config.h"

#include <QHash>
#include <QString>

namespace vramopt::application {

[[nodiscard]] const QHash<QString, domain::Confianca>& listaWrappersDesktop();

}
