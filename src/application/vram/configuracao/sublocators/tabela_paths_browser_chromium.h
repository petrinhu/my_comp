#pragma once

#include <QHash>
#include <QString>

namespace my_comp::application::vram {

[[nodiscard]] const QHash<QString, QString>& tabela_paths_browser_chromium();

}
