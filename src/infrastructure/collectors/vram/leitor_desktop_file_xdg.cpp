#include "infrastructure/collectors/vram/leitor_desktop_file_xdg.h"

#include "infrastructure/collectors/vram/desktop_xdg_search.h"

#include <utility>

namespace vramopt::infrastructure {

LeitorDesktopFileXdg::LeitorDesktopFileXdg(QStringList xdg_dirs)
    : m_xdg_dirs{xdg_dirs.isEmpty() ? desktop_xdg::xdgDirsDoSistema()
                                    : std::move(xdg_dirs)} {}

QStringList
LeitorDesktopFileXdg::categoriasDe(const domain::Processo& processo) const {
    const auto found = desktop_xdg::encontrarPorProcesso(m_xdg_dirs, processo);
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
