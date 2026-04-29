#pragma once

#include "back/classificacao/categoria.h"

#include <QString>
#include <QStringList>

namespace vramopt::domain {

struct EntradaCatalogo {
    QString     id{};
    QStringList binarios{};
    QStringList wm_class{};
    QStringList desktop_id{};
    Categoria   veredito{Categoria::Indeterminado};
    QString     justificativa{};
};

}
