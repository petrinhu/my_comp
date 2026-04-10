#include "systemd_service_provider.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QStringList>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

SystemdServiceProvider::SystemdServiceProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

std::vector<domain::models::ServiceInfo> SystemdServiceProvider::get_services() {
    auto system_units = collect_units(false);
    auto user_units = collect_units(true);
    
    std::vector<domain::models::ServiceInfo> all;
    all.reserve(system_units.size() + user_units.size());
    all.insert(all.end(), system_units.begin(), system_units.end());
    all.insert(all.end(), user_units.begin(), user_units.end());
    return all;
}

std::vector<domain::models::ServiceInfo> SystemdServiceProvider::collect_units(bool user_mode) {
    std::vector<domain::models::ServiceInfo> units;
    QString output;
    QStringList args = {"list-units", "--type=service", "--all", "--no-legend", "--no-pager"};
    if (user_mode) args.prepend("--user");
    
    if (m_executor->execute("systemctl", args, output) == 0) {
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            QStringList parts = line.trimmed().split(QRegularExpression("\\s+"));
            if (parts.size() >= 5) {
                domain::models::ServiceInfo info;
                info.name = parts[0];
                if (user_mode) info.name += " (User)";
                info.loadState = parts[1];
                info.activeState = parts[2];
                info.subState = parts[3];
                info.description = parts.mid(4).join(' ');
                
                // Verificar se esta habilitado
                QString enabledOutput;
                QStringList enabledArgs = {"is-enabled", parts[0]};
                if (user_mode) enabledArgs.prepend("--user");
                info.isEnabled = (m_executor->execute("systemctl", enabledArgs, enabledOutput) == 0);
                
                units.push_back(info);
            }
        }
    }
    return units;
}

bool SystemdServiceProvider::run_systemctl(const QStringList& args, bool user_mode) {
    QString output;
    QStringList finalArgs = args;
    if (user_mode) finalArgs.prepend("--user");
    return m_executor->execute("systemctl", finalArgs, output) == 0;
}

bool SystemdServiceProvider::start_service(const QString& name) {
    bool is_user = name.endsWith(" (User)");
    QString clean_name = name;
    if (is_user) clean_name.remove(" (User)");
    return run_systemctl({"enable", "--now", clean_name}, is_user);
}

bool SystemdServiceProvider::pause_service(const QString& name) {
    bool is_user = name.endsWith(" (User)");
    QString clean_name = name;
    if (is_user) clean_name.remove(" (User)");
    return run_systemctl({"stop", clean_name}, is_user);
}

bool SystemdServiceProvider::stop_service(const QString& name) {
    bool is_user = name.endsWith(" (User)");
    QString clean_name = name;
    if (is_user) clean_name.remove(" (User)");
    return run_systemctl({"disable", "--now", clean_name}, is_user);
}

bool SystemdServiceProvider::create_daemon(const QString& exe_path, const QString& params) {
    QFileInfo exeInfo(exe_path);
    QString serviceName = exeInfo.baseName().toLower() + "-daemon.service";
    
    QString configDir = QDir::homePath() + "/.config/systemd/user";
    QDir().mkpath(configDir);
    
    QFile file(configDir + "/" + serviceName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    
    QTextStream out(&file);
    out << "[Unit]\n";
    out << "Description=Daemon for " << exeInfo.fileName() << " (created by my_comp)\n";
    out << "After=network.target\n\n";
    out << "[Service]\n";
    out << "ExecStart=" << exe_path << " " << params << "\n";
    out << "Restart=always\n";
    out << "Type=simple\n\n";
    out << "[Install]\n";
    out << "WantedBy=default.target\n";
    file.close();
    
    // Reload e Iniciar
    run_systemctl({"daemon-reload"}, true);
    return run_systemctl({"enable", "--now", serviceName}, true);
}

} // namespace my_comp::infrastructure::collectors
