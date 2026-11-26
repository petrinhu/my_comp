// QA-03 — Aceitação: aba Serviços pela pilha real (ServiceViewModel → SystemdServiceProvider),
// com o systemctl roteirizado com saídas JSON reais (systemd 25x).
#include <gtest/gtest.h>
#include <QtTest/QSignalSpy>

#include "scripted_shell.h"
#include "../../src/infrastructure/collectors/systemd_service_provider.h"
#include "../../src/presentation/viewmodels/service_viewmodel.h"

using my_comp::infrastructure::collectors::SystemdServiceProvider;
using my_comp::presentation::viewmodels::ServiceViewModel;
using my_comp::tests::ScriptedShell;

namespace {
const QString LIST_UNITS = "systemctl list-units --type=service --all --no-pager --output=json";
const QString LIST_FILES = "systemctl list-unit-files --type=service --no-pager --output=json";

std::shared_ptr<ScriptedShell> fedora_desktop() {
    auto shell = std::make_shared<ScriptedShell>();
    shell->reply(LIST_UNITS, R"([{"unit":"cups.service","load":"loaded","active":"active","sub":"running","description":"CUPS Scheduler"}])");
    shell->reply(LIST_FILES, R"([{"unit_file":"cups.service","state":"enabled","preset":"enabled"}])");
    shell->reply("systemctl --user " + LIST_UNITS.mid(10), R"([{"unit":"syncthing.service","load":"loaded","active":"inactive","sub":"dead","description":"Syncthing"}])");
    shell->reply("systemctl --user " + LIST_FILES.mid(10), R"([{"unit_file":"syncthing.service","state":"disabled","preset":"enabled"}])");
    return shell;
}
} // namespace

TEST(SystemdServicesAcceptance, UserSeesSystemAndUserUnitsWithBootState) {
    auto shell = fedora_desktop();
    ServiceViewModel vm(std::make_shared<SystemdServiceProvider>(shell));
    QSignalSpy changed(&vm, &ServiceViewModel::servicesChanged);
    vm.refresh();
    ASSERT_TRUE(changed.wait(3000));

    const auto services = vm.services();
    ASSERT_EQ(services.size(), 2);
    QVariantMap cups, syncthing;
    for (const auto& s : services) {
        const auto m = s.toMap();
        (m["name"] == "cups.service" ? cups : syncthing) = m;
    }
    EXPECT_EQ(cups["scope"].toString(), QString("system"));
    EXPECT_TRUE(cups["isEnabled"].toBool());
    EXPECT_EQ(syncthing["scope"].toString(), QString("user"));
    EXPECT_FALSE(syncthing["isEnabled"].toBool());
}

TEST(SystemdServicesAcceptance, PermissionFailureReachesTheUser) {
    auto shell = fedora_desktop();
    shell->reply("systemctl disable --now cups.service", {}, 1,
                 "Failed to disable unit: Access denied\nSee system logs and 'systemctl status cups.service' for details.");
    ServiceViewModel vm(std::make_shared<SystemdServiceProvider>(shell));
    QSignalSpy finished(&vm, &ServiceViewModel::operationFinished);
    vm.disableAndStop("cups.service", "system");
    ASSERT_TRUE(finished.wait(3000));
    EXPECT_FALSE(finished.at(0).at(0).toBool());
    EXPECT_TRUE(finished.at(0).at(1).toString().contains("Access denied"));
}

TEST(SystemdServicesAcceptance, UserUnitsAreControlledWithUserFlag) {
    auto shell = fedora_desktop();
    shell->reply("systemctl --user enable --now syncthing.service", {});
    ServiceViewModel vm(std::make_shared<SystemdServiceProvider>(shell));
    QSignalSpy finished(&vm, &ServiceViewModel::operationFinished);
    vm.enableAndStart("syncthing.service", "user");
    ASSERT_TRUE(finished.wait(3000));
    EXPECT_TRUE(finished.at(0).at(0).toBool());
    EXPECT_TRUE(shell->calls().contains("systemctl --user enable --now syncthing.service"));
}
