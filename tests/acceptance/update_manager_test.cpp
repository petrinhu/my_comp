#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <memory>
#include "../../src/domain/interfaces/i_update_provider.h"
#include "../../src/domain/models/update_package.h"
#include "../../src/domain/update_manager.h"

using namespace my_comp::domain;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

// Mock de um provedor de atualização para o teste de aceitação
class MockUpdateProvider : public IUpdateProvider {
public:
    MOCK_METHOD(QString, get_name, (), (const, override));
    MOCK_METHOD(bool, is_available, (), (const, override));
    MOCK_METHOD(std::vector<UpdatePackage>, check_updates, (), (override));
};

TEST(UpdateManagerATDD, ShouldConsolidateUpdatesFromMultipleProviders) {
    auto manager = std::make_unique<UpdateManager>();

    auto dnf_mock = std::make_unique<MockUpdateProvider>();
    auto npm_mock = std::make_unique<MockUpdateProvider>();

    EXPECT_CALL(*dnf_mock, is_available()).WillRepeatedly(Return(true));
    EXPECT_CALL(*dnf_mock, check_updates()).WillOnce(Return(std::vector<UpdatePackage>{
        {"kernel", "6.12.0", "6.13.0", "dnf", "Security fix", true}
    }));

    EXPECT_CALL(*npm_mock, is_available()).WillRepeatedly(Return(true));
    EXPECT_CALL(*npm_mock, check_updates()).WillOnce(Return(std::vector<UpdatePackage>{
        {"typescript", "5.0.0", "5.1.0", "npm", "New features", false}
    }));

    manager->register_provider(std::move(dnf_mock));
    manager->register_provider(std::move(npm_mock));

    auto all_updates = manager->check_all_updates();

    ASSERT_EQ(all_updates.size(), 2);
    EXPECT_EQ(all_updates[0].name, "kernel");
    EXPECT_EQ(all_updates[1].name, "typescript");
}
