#pragma once

#include "domain/vram/entidades/processo.h"

#include <QHash>
#include <QString>
#include <QStringList>

#include <optional>

namespace vramopt::infrastructure::desktop_xdg {

struct ArquivoDesktopParseado {
    QString                 caminho;
    QHash<QString, QString> entrada_desktop;
};

[[nodiscard]] QStringList xdgDirsDoSistema();

[[nodiscard]] std::optional<ArquivoDesktopParseado>
    encontrarPorProcesso(const QStringList& xdg_dirs,
                         const domain::Processo& processo);

}
