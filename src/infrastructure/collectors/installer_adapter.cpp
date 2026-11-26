#include "installer_adapter.h"
#include <QFile>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

namespace {
// Elevação via polkit: sudo numa GUI não tem TTY para pedir senha (lê de /dev/tty, não do stdin).
constexpr QLatin1String ELEVATE{"pkexec"}; // constexpr: sem init global que lança (cert-err58)
constexpr int REJECTED_EXIT_CODE = 2;
constexpr qsizetype MAX_PACKAGE_NAME_LENGTH = 256;
} // namespace

bool is_safe_package_name(const QString& name) {
    // 1º caractere alfanumérico ou '@' (escopo npm); depois o alfabeto usual de nomes
    // RPM/Debian/Arch/Flatpak/npm/crates: letras, dígitos e . _ + - : @ /
    static const QRegularExpression allowed(QStringLiteral("^[A-Za-z0-9@][A-Za-z0-9._+:@/-]*$"));
    return !name.isEmpty() && name.size() <= MAX_PACKAGE_NAME_LENGTH && allowed.match(name).hasMatch();
}

InstallerAdapter::InstallerAdapter(std::shared_ptr<domain::interfaces::IInteractiveRunner> runner)
    : m_runner(std::move(runner)) {}

void InstallerAdapter::set_output_handler(std::function<void(const QString&)> handler) {
    m_on_output = handler;
    m_runner->set_output_handler(std::move(handler));
}

void InstallerAdapter::set_interaction_handler(std::function<void(const QString&)> handler) {
    m_runner->set_interaction_handler(std::move(handler));
}

void InstallerAdapter::set_finished_handler(std::function<void(int)> handler) {
    m_on_finished = handler;
    m_runner->set_finished_handler(std::move(handler));
}

void InstallerAdapter::reject(const QString& reason) {
    if (m_on_output) m_on_output(reason);
    if (m_on_finished) m_on_finished(REJECTED_EXIT_CODE);
}

void InstallerAdapter::install(const domain::models::UpdatePackage& package) {
    if (!is_safe_package_name(package.name)) {
        reject(QStringLiteral("Instalação recusada: nome de pacote inválido \"%1\".\n").arg(package.name));
        return;
    }

    QString cmd;
    QStringList args;

    // "--" encerra o parsing de opções nos parsers que o suportam (getopt/GOption/go-flags/
    // nopt/clap). Em dnf e apt a defesa é o is_safe_package_name (nunca começa por '-').
    switch (package.provider) {
        case domain::models::ProviderType::Dnf:
            cmd = ELEVATE;
            args = {"dnf", "install", "-y", package.name};
            break;
        case domain::models::ProviderType::Flatpak:
            cmd = "flatpak"; // escopo do usuário; o flatpak pede polkit sozinho quando precisa
            args = {"install", "-y", "--", package.name};
            break;
        case domain::models::ProviderType::Snap:
            cmd = ELEVATE;
            args = {"snap", "refresh", "--", package.name};
            break;
        case domain::models::ProviderType::Cargo:
            cmd = "cargo"; // instala em ~/.cargo, sem elevação
            args = {"install-update", "--", package.name};
            break;
        case domain::models::ProviderType::Apt:
            cmd = ELEVATE;
            args = {"apt", "install", "-y", package.name};
            break;
        case domain::models::ProviderType::Pacman:
            cmd = ELEVATE;
            args = {"pacman", "-S", "--noconfirm", "--", package.name};
            break;
        case domain::models::ProviderType::Npm:
            cmd = ELEVATE;
            args = {"npm", "install", "-g", "--", package.name};
            break;
    }
    m_runner->start(cmd, args);
}

void InstallerAdapter::upgradeAll() {
    QStringList args;

    if (QFile::exists("/usr/bin/dnf")) {
        args = {"dnf", "upgrade", "-y"};
    } else if (QFile::exists("/usr/bin/apt")) {
        args = {"apt", "upgrade", "-y"};
    } else if (QFile::exists("/usr/bin/pacman")) {
        args = {"pacman", "-Syu", "--noconfirm"};
    } else {
        return;
    }

    m_runner->start(ELEVATE, args);
}

void InstallerAdapter::sendResponse(const QString& response) {
    m_runner->sendResponse(response);
}

} // namespace my_comp::infrastructure::collectors
