#pragma once

#include "domain/vram/configuracao/origem_config.h"

#include <QHash>
#include <QString>

namespace vramopt::application {

[[nodiscard]] const QHash<QString, domain::Confianca>& lista_wrappers_desktop();

}
