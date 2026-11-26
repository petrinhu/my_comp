#pragma once

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <vector>

namespace my_comp::infrastructure::collectors {

// Uma ferramenta detectável: nome exibido + comando que imprime a versão (stdout ou stderr).
struct ToolchainSpec {
    QString name;
    QString command;
    QStringList args;
};

// Catálogo embutido (DT-01a): compiladores, runtimes, build tools e CLIs de cluster comuns.
[[nodiscard]] std::vector<ToolchainSpec> default_toolchain_catalog();

// JSON do usuário: [{"name": "...", "command": "...", "args": ["..."]}, ...]. Entrada externa
// (MC-10): entradas sem campos obrigatórios, com tipos errados ou caracteres de controle são
// descartadas; JSON malformado vira lista vazia.
[[nodiscard]] std::vector<ToolchainSpec> parse_toolchain_catalog(const QByteArray& json);

// Mesmo `name` no catálogo do usuário sobrescreve o padrão; nomes novos vão para o fim.
[[nodiscard]] std::vector<ToolchainSpec> merge_toolchain_catalogs(std::vector<ToolchainSpec> base,
                                                                  const std::vector<ToolchainSpec>& user);

// Padrão + `<AppConfigLocation>/toolchains.json`, se existir (lido no composition root).
[[nodiscard]] std::vector<ToolchainSpec> load_toolchain_catalog();

} // namespace my_comp::infrastructure::collectors
