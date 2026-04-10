#pragma once
#include "../../domain/interfaces/i_user_metadata_provider.h"
#include "i_shell_executor.h"
#include <memory>

namespace my_comp::infrastructure::collectors {

class LinuxUserMetadataProvider : public domain::interfaces::IUserMetadataProvider {
public:
    explicit LinuxUserMetadataProvider(std::shared_ptr<IShellExecutor> executor);
    domain::models::UserMetadata get_user_metadata() override;

private:
    std::shared_ptr<IShellExecutor> m_executor;
    
    void collect_current_user(domain::models::UserMetadata& metadata);
    void collect_env_vars(domain::models::UserMetadata& metadata);
    void collect_limits(domain::models::UserMetadata& metadata);
    void collect_login_history(domain::models::UserMetadata& metadata);
    void collect_system_users(domain::models::UserMetadata& metadata);
};

} // namespace my_comp::infrastructure::collectors
