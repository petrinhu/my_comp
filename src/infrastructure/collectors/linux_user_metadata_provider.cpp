#include "linux_user_metadata_provider.h"
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <vector>
#include <QStringList>
#include <QProcess>
#include <cstdlib>
#include <mutex>

namespace my_comp::infrastructure::collectors {

static std::mutex g_user_mutex; // Mutex global para funções libc não reentrantes

LinuxUserMetadataProvider::LinuxUserMetadataProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

domain::models::UserMetadata LinuxUserMetadataProvider::get_user_metadata() {
    domain::models::UserMetadata metadata;
    collect_current_user(metadata);
    collect_env_vars(metadata);
    collect_limits(metadata);
    collect_login_history(metadata);
    collect_system_users(metadata);
    return metadata;
}

void LinuxUserMetadataProvider::collect_current_user(domain::models::UserMetadata& metadata) {
    uid_t uid = getuid();
    gid_t gid = getgid();
    
    struct passwd pwd;
    struct passwd *result;
    char buffer[1024];
    
    if (getpwuid_r(uid, &pwd, buffer, sizeof(buffer), &result) == 0 && result != nullptr) {
        metadata.currentUser = QString::fromLocal8Bit(pwd.pw_name);
        metadata.currentUid = QString::number(uid);
        metadata.currentGid = QString::number(gid);
        metadata.shell = QString::fromLocal8Bit(pwd.pw_shell);
        metadata.home = QString::fromLocal8Bit(pwd.pw_dir);
    }
    
    int n_groups = getgroups(0, NULL);
    if (n_groups > 0) {
        std::vector<gid_t> groups(n_groups);
        getgroups(n_groups, groups.data());
        for (int i = 0; i < n_groups; ++i) {
            struct group grp;
            struct group *gr_result;
            char gr_buffer[1024];
            if (getgrgid_r(groups[i], &grp, gr_buffer, sizeof(gr_buffer), &gr_result) == 0 && gr_result != nullptr) {
                metadata.groups << QString::fromLocal8Bit(grp.gr_name);
            }
        }
    }
}

void LinuxUserMetadataProvider::collect_env_vars(domain::models::UserMetadata& metadata) {
    const char* env_vars[] = {
        "LANG", "PATH", "DISPLAY", "WAYLAND_DISPLAY", "XDG_SESSION_TYPE",
        "XDG_CURRENT_DESKTOP", "SHELL", "HOME", "USER", "DESKTOP_SESSION"
    };
    
    for (const char* var : env_vars) {
        const char* val = std::getenv(var);
        if (val) {
            metadata.envVars[QString::fromLocal8Bit(var)] = QString::fromLocal8Bit(val);
        } else {
            metadata.envVars[QString::fromLocal8Bit(var)] = "not set";
        }
    }
}

void LinuxUserMetadataProvider::collect_limits(domain::models::UserMetadata& metadata) {
    QString output;
    if (m_executor->execute("bash", {"-c", "ulimit -a"}, output) == 0) {
        metadata.systemLimits = output.split('\n', Qt::SkipEmptyParts);
    }
}

void LinuxUserMetadataProvider::collect_login_history(domain::models::UserMetadata& metadata) {
    QString output;
    if (m_executor->execute("last", {"-n", "20"}, output) == 0) {
        metadata.loginHistory = output.split('\n', Qt::SkipEmptyParts);
    }
}

void LinuxUserMetadataProvider::collect_system_users(domain::models::UserMetadata& metadata) {
    std::lock_guard<std::mutex> lock(g_user_mutex);
    setpwent();
    struct passwd *pw;
    while ((pw = getpwent()) != NULL) {
        if (pw->pw_uid >= 1000 && pw->pw_uid < 65534) {
            QString user_info = QString("%1 (UID:%2, Shell:%3, Home:%4)")
                .arg(QString::fromLocal8Bit(pw->pw_name))
                .arg(pw->pw_uid)
                .arg(QString::fromLocal8Bit(pw->pw_shell))
                .arg(QString::fromLocal8Bit(pw->pw_dir));
            metadata.systemUsers << user_info;
        }
    }
    endpwent();
}

} // namespace my_comp::infrastructure::collectors
