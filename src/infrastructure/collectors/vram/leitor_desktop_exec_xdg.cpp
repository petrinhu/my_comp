#include "infrastructure/collectors/vram/leitor_desktop_exec_xdg.h"

#include "infrastructure/collectors/vram/desktop_xdg_search.h"

#include <utility>

namespace my_comp::infrastructure::vram {

LeitorDesktopExecXdg::LeitorDesktopExecXdg(QStringList xdg_dirs)
    : m_xdg_dirs{xdg_dirs.isEmpty() ? desktop_xdg::xdg_dirs_do_sistema()
                                    : std::move(xdg_dirs)} {}

std::optional<my_comp::domain::vram::EntradaDesktopExec>
LeitorDesktopExecXdg::entrada_de(const domain::vram::Processo& processo) const {
    const auto found = desktop_xdg::encontrar_por_processo(m_xdg_dirs, processo);
    if (!found.has_value()) {
        return std::nullopt;
    }
    const auto exec = found->entrada_desktop.value(QStringLiteral("Exec"));
    if (exec.isEmpty()) {
        return std::nullopt;
    }
    return domain::vram::EntradaDesktopExec{
        .exec = exec,
        .caminho_arquivo = found->caminho,
    };
}

}
