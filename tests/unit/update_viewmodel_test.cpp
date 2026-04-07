#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>
#include "../../src/domain/update_manager.h"
#include "../../src/domain/interfaces/i_update_provider.h"
#include "../../src/domain/models/update_package.h"
#include "../../src/presentation/viewmodels/update_viewmodel.h"

using namespace my_comp::domain;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace my_comp::presentation::viewmodels;
using namespace testing;

class MockUpdateProvider : public IUpdateProvider {
public:
    MOCK_METHOD(QString, get_name, (), (const, override));
    MOCK_METHOD(bool, is_available, (), (const, override));
    MOCK_METHOD(std::vector<UpdatePackage>, check_updates, (), (override));
};

TEST(UpdateViewModelTest, ShouldRefreshUpdatesFromManager) {
    auto manager = std::make_shared<UpdateManager>();
    auto mock_provider = std::make_unique<MockUpdateProvider>();

    EXPECT_CALL(*mock_provider, is_available()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_provider, check_updates()).WillOnce(Return(std::vector<UpdatePackage>{
        {"test-pkg", "1.0", "1.1", "test", "desc", false}
    }));

    manager->register_provider(std::move(mock_provider));

    UpdateViewModel vm(manager);
    vm.refresh();

    EXPECT_EQ(vm.totalUpdates(), 1);
    EXPECT_FALSE(vm.isBusy());
}

#include "update_viewmodel_test.moc"

