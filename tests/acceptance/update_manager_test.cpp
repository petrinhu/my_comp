#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <memory>
#include "../../src/domain/update_manager.h"
#include "../../src/domain/interfaces/i_update_provider.h"
#include "../../src/domain/models/update_package.h"

using namespace my_comp::domain;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockUpdateProvider : public IUpdateProvider {
public:
    MOCK_METHOD(QString, get_name, (), (const, override));
    MOCK_METHOD(bool, is_available, (), (const, override));
    MOCK_METHOD(std::vector<UpdatePackage>, check_updates, (), (override));
};

#include "../../src/domain/interfaces/i_installer.h"

class MockInstaller : public IInstaller {
public:
    MOCK_METHOD(void, install, (const UpdatePackage&), (override));
    MOCK_METHOD(void, upgradeAll, (), (override));
    MOCK_METHOD(void, sendResponse, (const QString&), (override));
    void set_output_handler(std::function<void(const QString&)>) override {}
    void set_interaction_handler(std::function<void(const QString&)>) override {}
    void set_finished_handler(std::function<void(int)>) override {}
};

TEST(UpdateManagerATDD, ShouldOrchestrateFullUpgrade) {
    auto manager = std::make_shared<UpdateManager>();
    auto mock_installer = std::make_shared<MockInstaller>();

    manager->set_installer(mock_installer);

    // Mandato: O Manager deve chamar upgradeAll do instalador
    EXPECT_CALL(*mock_installer, upgradeAll()).Times(1);

    manager->upgrade_all();
}

TEST(UpdateManagerATDD, ShouldConsolidateUpdatesFromMultipleProviders) {
    auto manager = std::make_shared<UpdateManager>();
    
    auto provider1 = std::make_unique<MockUpdateProvider>();
    auto provider2 = std::make_unique<MockUpdateProvider>();

    UpdatePackage pkg1; pkg1.name = "pkg1"; pkg1.source = "dnf"; pkg1.provider = ProviderType::Dnf;
    UpdatePackage pkg2; pkg2.name = "pkg2"; pkg2.source = "flatpak"; pkg2.provider = ProviderType::Flatpak;

    EXPECT_CALL(*provider1, is_available()).WillRepeatedly(Return(true));
    EXPECT_CALL(*provider1, check_updates()).WillOnce(Return(std::vector<UpdatePackage>{pkg1}));
    
    EXPECT_CALL(*provider2, is_available()).WillRepeatedly(Return(true));
    EXPECT_CALL(*provider2, check_updates()).WillOnce(Return(std::vector<UpdatePackage>{pkg2}));

    manager->register_provider(std::move(provider1));
    manager->register_provider(std::move(provider2));

    auto updates = manager->check_all_updates();

    ASSERT_EQ(updates.size(), 2);
}
