#ifndef DNF_UPDATE_PROVIDER_H
#define DNF_UPDATE_PROVIDER_H

#include "../../domain/interfaces/i_update_provider.h"
#include "i_shell_executor.h"
#include <QRegularExpression>
#include <QStringList>
#include <QHash>
#include <memory>

namespace my_comp::infrastructure::collectors {

class DnfUpdateProvider : public domain::interfaces::IUpdateProvider {
public:
    explicit DnfUpdateProvider(std::shared_ptr<IShellExecutor> shell) : m_shell(std::move(shell)) {}

    [[nodiscard]] QString get_name() const override { return "dnf"; }

    [[nodiscard]] bool is_available() const override {
        QString output;
        m_shell->execute("which", {"dnf"}, output);
        return !output.isEmpty();
    }

    [[nodiscard]] std::vector<domain::models::UpdatePackage> check_updates() override {
        QString output;
        std::vector<domain::models::UpdatePackage> updates;

        if (m_shell->execute_with_timeout("dnf", {"check-update", "--quiet"}, output, PACKAGE_MANAGER_TIMEOUT_MS) == 100) {
            auto lines = output.split('\n', Qt::SkipEmptyParts);
            for (const auto& line : lines) {
                auto parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                if (parts.size() >= 3) {
                    domain::models::UpdatePackage pkg;
                    pkg.name = parts[0];
                    pkg.new_version = parts[1];
                    pkg.source = "dnf";
                    pkg.provider = domain::models::ProviderType::Dnf;
                    pkg.source_repo = parts[2];
                    pkg.description = parts[2]; // compat: UI ainda exibe description
                    pkg.is_critical = parts[0].contains("kernel");
                    updates.push_back(pkg);
                }
            }
            enrich_current_versions(updates);
        }
        return updates;
    }

private:
    void enrich_current_versions(std::vector<domain::models::UpdatePackage>& updates) const {
        if (updates.empty()) return;
        QStringList names;
        names.reserve(static_cast<int>(updates.size()));
        for (const auto& u : updates) {
            // "name.arch" → ficar só com "name" (rpm -q aceita ambos, mas o match vira name-version)
            names << u.name.section('.', 0, 0);
        }
        QString rpm_out;
        QStringList args;
        args << "-q" << "--qf" << "%{NAME} %{VERSION}-%{RELEASE}\n";
        args.append(names);
        if (m_shell->execute("rpm", args, rpm_out) != 0) return;
        QHash<QString, QString> installed;
        for (const auto& line : rpm_out.split('\n', Qt::SkipEmptyParts)) {
            const auto sp = line.split(' ', Qt::SkipEmptyParts);
            if (sp.size() == 2) installed.insert(sp[0], sp[1]);
        }
        for (auto& u : updates) {
            const QString key = u.name.section('.', 0, 0);
            if (installed.contains(key)) u.current_version = installed.value(key);
        }
    }

    std::shared_ptr<IShellExecutor> m_shell;
};

} // namespace my_comp::infrastructure::collectors

#endif // DNF_UPDATE_PROVIDER_H
