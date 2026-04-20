#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include <memory>
#include <vector>
#include "../../src/application/update_manager.h"
#include "../../src/domain/models/update_package.h"
#include "../../src/presentation/viewmodels/update_viewmodel.h"

using namespace my_comp::application;
using namespace my_comp::domain::models;
using namespace my_comp::presentation::viewmodels;
using namespace testing;

class MockUpdateManager : public UpdateManager {
public:
    MOCK_METHOD(std::vector<UpdatePackage>, check_all_updates, (), (override));
};

TEST(UpdateViewModelTest, ShouldRefreshUpdatesFromManager) {
    auto manager = std::make_shared<MockUpdateManager>();
    UpdateViewModel vm(manager);

    UpdatePackage pkg; pkg.name = "test-pkg"; pkg.source = "dnf"; pkg.provider = ProviderType::Dnf;

    EXPECT_CALL(*manager, check_all_updates())
        .WillOnce(Return(std::vector<UpdatePackage>{pkg}));

    // refresh() é async (QtConcurrent + QueuedConnection). Aguardar isBusy voltar a false.
    QSignalSpy busySpy(&vm, &UpdateViewModel::isBusyChanged);
    vm.refresh();
    ASSERT_TRUE(busySpy.wait(2000)); // transição true
    if (vm.isBusy()) busySpy.wait(2000); // transição false pós-QtConcurrent

    EXPECT_FALSE(vm.isBusy());
    EXPECT_EQ(vm.totalUpdates(), 1);
}
