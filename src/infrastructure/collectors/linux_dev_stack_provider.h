#pragma once
#include "../../domain/interfaces/i_dev_stack_provider.h"
#include "i_shell_executor.h"
#include "toolchain_catalog.h"
#include <QJsonObject>
#include <memory>

namespace my_comp::infrastructure::collectors {

class LinuxDevStackProvider : public domain::interfaces::IDevStackProvider {
public:
    // catalog: ferramentas a detectar (padrão: default_toolchain_catalog()).
    explicit LinuxDevStackProvider(std::shared_ptr<IShellExecutor> executor,
                                   std::vector<ToolchainSpec> catalog = default_toolchain_catalog());
    domain::models::DevStackInfo collect_dev_stacks() override;

    // Públicos estáticos — pura lógica de parsing, expostos para fuzzing/teste isolado (BT-02).
    static void parse_containers_json(const QString& output, const QString& runtime, std::vector<domain::models::ContainerInfo>& containers);
    static void parse_container_object(const QJsonObject& obj, const QString& runtime, std::vector<domain::models::ContainerInfo>& containers);

private:
    std::shared_ptr<IShellExecutor> m_executor;
    std::vector<ToolchainSpec> m_catalog;

    void collect_toolchains(domain::models::DevStackInfo& info);
    void collect_containers(domain::models::DevStackInfo& info);

    QString get_version(const QString& cmd, const QStringList& args);
};

} // namespace my_comp::infrastructure::collectors
