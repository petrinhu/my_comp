#pragma once

#include "back/configuracao/entrada_desktop_exec.h"
#include "back/entidades/processo.h"

#include <optional>

namespace vramopt::domain {

class ILeitorDesktopExec {
public:
    ILeitorDesktopExec() = default;
    ILeitorDesktopExec(const ILeitorDesktopExec&) = delete;
    ILeitorDesktopExec(ILeitorDesktopExec&&) = delete;
    ILeitorDesktopExec& operator=(const ILeitorDesktopExec&) = delete;
    ILeitorDesktopExec& operator=(ILeitorDesktopExec&&) = delete;
    virtual ~ILeitorDesktopExec() = default;

    [[nodiscard]] virtual std::optional<EntradaDesktopExec>
        entradaDe(const Processo& processo) const = 0;
};

}
