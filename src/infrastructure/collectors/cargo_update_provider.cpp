#include "cargo_update_provider.h"
#include <QStringList>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

CargoUpdateProvider::CargoUpdateProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

std::vector<domain::models::UpdatePackage> CargoUpdateProvider::check_updates() {
    std::vector<domain::models::UpdatePackage> updates;
    QString output;
    
    // Requer 'cargo install-update' (crate cargo-update)
    // Output: package-name  current-version  latest-version
    if (m_executor->execute_with_timeout("cargo", {"install-update", "-l"}, output, PACKAGE_MANAGER_TIMEOUT_MS) == 0) {
        const QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            // Ignorar linhas informativas que nao começam com dados de pacote
            if (line.contains("Package") || line.contains("---")) continue;
            
            QStringList parts = line.trimmed().split(QRegularExpression("\\s+"));
            if (parts.size() >= 3) {
                domain::models::UpdatePackage pkg;
                pkg.name = parts[0];
                pkg.current_version = parts[1];
                pkg.new_version = parts[2];
                pkg.source = "cargo";
                pkg.provider = domain::models::ProviderType::Cargo;
                updates.push_back(pkg);
            }
        }
    }
    return updates;
}

bool CargoUpdateProvider::is_available() const {
    QString output;
    // IN-08a: cargo sozinho não basta — subcomando `install-update` exige crate `cargo-update`.
    if (m_executor->execute("cargo", {"--version"}, output) != 0) return false;
    QString help_out;
    m_executor->execute("cargo", {"install-update", "--help"}, help_out);
    // Quando o subcomando existe, `--help` retorna texto de uso; quando não, cargo diz
    // "no such command: install-update" (no stderr, já capturado pelo executor).
    return help_out.contains("install-update") || help_out.contains("Usage");
}

} // namespace my_comp::infrastructure::collectors
