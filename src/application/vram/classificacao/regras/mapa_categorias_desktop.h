#pragma once

#include "domain/vram/classificacao/categoria.h"

#include <QSet>
#include <QString>
#include <QStringList>

#include <optional>

namespace vramopt::application {

class MapaCategoriasDesktop {
public:
    MapaCategoriasDesktop(QSet<QString> candidato_cpu, QSet<QString> precisa_gpu);

    [[nodiscard]] static MapaCategoriasDesktop padrao();

    [[nodiscard]] std::optional<domain::Categoria>
        classificar_por(const QStringList& categorias) const;

private:
    QSet<QString> m_candidato_cpu;
    QSet<QString> m_precisa_gpu;
};

}
