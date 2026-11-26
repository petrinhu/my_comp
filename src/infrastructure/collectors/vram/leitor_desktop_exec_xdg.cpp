#include "infrastructure/collectors/vram/leitor_desktop_exec_xdg.h"

#include "infrastructure/collectors/vram/desktop_xdg_search.h"

#include <utility>

namespace vramopt::infrastructure {

LeitorDesktopExecXdg::LeitorDesktopExecXdg(QStringList xdg_dirs)
    : m_xdg_dirs{xdg_dirs.isEmpty() ? desktop_xdg::xdgDirsDoSistema()
                                    : std::move(xdg_dirs)} {}

std::optional<vramopt::domain::EntradaDesktopExec>
LeitorDesktopExecXdg::entrada_de(const domain::Processo& processo) const {
    const auto found = desktop_xdg::encontrarPorProcesso(m_xdg_dirs, processo);
    if (!found.has_value()) {
        return std::nullopt;
    }
    const auto exec = found->entrada_desktop.value(QStringLiteral("Exec"));
    if (exec.isEmpty()) {
        return std::nullopt;
    }
    return domain::EntradaDesktopExec{
        .exec = exec,
        .caminho_arquivo = found->caminho,
    };
}

}
