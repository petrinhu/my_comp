#include "infrastructure/collectors/vram/leitor_desktop_file_xdg.h"

#include "infrastructure/collectors/vram/desktop_xdg_search.h"

#include <utility>

namespace my_comp::infrastructure::vram {

LeitorDesktopFileXdg::LeitorDesktopFileXdg(QStringList xdg_dirs)
    : m_xdg_dirs{xdg_dirs.isEmpty() ? desktop_xdg::xdg_dirs_do_sistema()
                                    : std::move(xdg_dirs)} {}

QStringList
LeitorDesktopFileXdg::categorias_de(const domain::vram::Processo& processo) const {
    const auto found = desktop_xdg::encontrar_por_processo(m_xdg_dirs, processo);
    if (!found.has_value()) {
        return {};
    }
    const auto cats = found->entrada_desktop.value(QStringLiteral("Categories"));
    if (cats.isEmpty()) {
        return {};
    }
    return cats.split(QLatin1Char(';'), Qt::SkipEmptyParts);
}

}
