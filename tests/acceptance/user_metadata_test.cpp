// QA-03 — Aceitação: aba Usuário pela pilha real (UserMetadataViewModel →
// LinuxUserMetadataProvider) contra o sistema (getpwuid_r, ambiente).
#include <gtest/gtest.h>
#include <QtTest/QSignalSpy>
#include <pwd.h>
#include <unistd.h>

#include "../../src/infrastructure/collectors/linux_user_metadata_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"
#include "../../src/presentation/viewmodels/user_metadata_viewmodel.h"

using namespace my_comp::infrastructure::collectors;
using my_comp::presentation::viewmodels::UserMetadataViewModel;

TEST(UserMetadataAcceptance, UserSeesOwnAccount) {
    UserMetadataViewModel vm(std::make_shared<LinuxUserMetadataProvider>(std::make_shared<QProcessShellExecutor>()));
    QSignalSpy changed(&vm, &UserMetadataViewModel::dataChanged);
    vm.refresh();
    if (changed.isEmpty()) {
        ASSERT_TRUE(changed.wait(5000));
    }

    const passwd* pw = ::getpwuid(::getuid());
    ASSERT_NE(pw, nullptr);
    EXPECT_EQ(vm.currentUser(), QString::fromLocal8Bit(pw->pw_name));
    EXPECT_EQ(vm.home(), QString::fromLocal8Bit(pw->pw_dir));
    EXPECT_FALSE(vm.envVars().isEmpty());
}
