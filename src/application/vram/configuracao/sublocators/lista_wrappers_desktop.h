#pragma once

#include "domain/vram/configuracao/origem_config.h"

#include <QHash>
#include <QString>

namespace my_comp::application::vram {

[[nodiscard]] const QHash<QString, domain::vram::Confianca>& lista_wrappers_desktop();

}
