#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/installer_adapter.h"

#include <QStringList>

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain;

namespace {

// Runner falso: registra o comando em vez de executar.
class RecordingRunner : public interfaces::IInteractiveRunner {
public:
    void start(const QString& cmd, const QStringList& args) override {
        ++starts;
        last_cmd = cmd;
        last_args = args;
    }
    void sendResponse(const QString&) override {}
    void set_output_handler(std::function<void(const QString&)> h) override { on_output = std::move(h); }
    void set_interaction_handler(std::function<void(const QString&)>) override {}
    void set_finished_handler(std::function<void(int)> h) override { on_finished = std::move(h); }

    int starts{0};
    QString last_cmd;
    QStringList last_args;
    std::function<void(const QString&)> on_output;
    std::function<void(int)> on_finished;
};

models::UpdatePackage pkg(models::ProviderType type, const QString& name) {
    models::UpdatePackage p;
    p.name = name;
    p.provider = type;
    return p;
}

} // namespace

// sudo numa GUI não tem TTY para pedir senha: elevação precisa ser via polkit (pkexec).
TEST(InstallerAdapterTest, ElevatesWithPkexecInsteadOfSudo) {
    auto runner = std::make_shared<RecordingRunner>();
    InstallerAdapter adapter(runner);

    for (auto type : {models::ProviderType::Dnf, models::ProviderType::Apt,
                      models::ProviderType::Pacman, models::ProviderType::Npm,
                      models::ProviderType::Snap}) {
        adapter.install(pkg(type, "htop"));
        EXPECT_EQ(runner->last_cmd, "pkexec");
        EXPECT_FALSE(runner->last_args.contains("sudo"));
        EXPECT_EQ(runner->last_args.last(), "htop");
    }
}

TEST(InstallerAdapterTest, UserScopedManagersDoNotElevate) {
    auto runner = std::make_shared<RecordingRunner>();
    InstallerAdapter adapter(runner);

    adapter.install(pkg(models::ProviderType::Flatpak, "org.kde.kate"));
    EXPECT_EQ(runner->last_cmd, "flatpak");
    adapter.install(pkg(models::ProviderType::Cargo, "ripgrep"));
    EXPECT_EQ(runner->last_cmd, "cargo");
}

// Nome de pacote vem do parsing da saída de dnf/apt/etc. (entrada externa, MC-10):
// "-x"/"--opção" viraria injeção de opção no gerenciador rodando como root.
TEST(InstallerAdapterTest, RejectsPackageNamesThatLookLikeOptions) {
    auto runner = std::make_shared<RecordingRunner>();
    InstallerAdapter adapter(runner);
    int finished_code = 0;
    QString message;
    adapter.set_finished_handler([&](int code) { finished_code = code; });
    adapter.set_output_handler([&](const QString& m) { message = m; });

    for (const QString bad : {"--setopt=gpgcheck=0", "-y", "", "pkg name", "pkg;rm", "a\nb", "$(id)"}) {
        runner->starts = 0;
        finished_code = 0;
        adapter.install(pkg(models::ProviderType::Dnf, bad));
        EXPECT_EQ(runner->starts, 0) << bad.toStdString();
        EXPECT_NE(finished_code, 0) << bad.toStdString();
    }
    EXPECT_FALSE(message.isEmpty());
}

TEST(InstallerAdapterTest, AcceptsRealWorldPackageNames) {
    for (const QString ok : {"htop", "python3.12-libs.x86_64", "libstdc++", "kernel-core",
                             "org.kde.kate", "@angular/cli", "g++-13", "perl:Some::Mod"}) {
        EXPECT_TRUE(is_safe_package_name(ok)) << ok.toStdString();
    }
    for (const QString bad : {"-rf", "--force", "", " lead", "x y", "a|b", "a`b"}) {
        EXPECT_FALSE(is_safe_package_name(bad)) << bad.toStdString();
    }
}

// getopt-style: "--" encerra opções antes do nome onde o parser suporta.
TEST(InstallerAdapterTest, EndsOptionParsingBeforeNameWhereSupported) {
    auto runner = std::make_shared<RecordingRunner>();
    InstallerAdapter adapter(runner);
    for (auto type : {models::ProviderType::Pacman, models::ProviderType::Flatpak,
                      models::ProviderType::Snap, models::ProviderType::Npm,
                      models::ProviderType::Cargo}) {
        adapter.install(pkg(type, "htop"));
        ASSERT_GE(runner->last_args.size(), 2);
        EXPECT_EQ(runner->last_args.at(runner->last_args.size() - 2), "--");
    }
}

TEST(InstallerAdapterTest, UpgradeAllElevatesWithPkexec) {
    auto runner = std::make_shared<RecordingRunner>();
    InstallerAdapter adapter(runner);
    adapter.upgradeAll();
    if (runner->starts > 0) {
        EXPECT_EQ(runner->last_cmd, "pkexec");
    }
}
