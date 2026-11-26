#pragma once

#include "domain/vram/classificacao/categoria.h"

#include <QString>
#include <QStringList>

namespace my_comp::domain::vram {

struct EntradaCatalogo {
    QString     id{};
    QStringList binarios{};
    QStringList wm_class{};
    QStringList desktop_id{};
    Categoria   veredito{Categoria::Indeterminado};
    QString     justificativa{};
};

}
