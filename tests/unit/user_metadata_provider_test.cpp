#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../../src/infrastructure/collectors/linux_user_metadata_provider.h"
#include "mock_shell_executor.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::tests;
using namespace testing;

class UserMetadataProviderTest : public Test {
protected:
    std::shared_ptr<MockShellExecutor> executor;
    std::unique_ptr<LinuxUserMetadataProvider> provider;

    void SetUp() override {
        executor = std::make_shared<MockShellExecutor>();
        provider = std::make_unique<LinuxUserMetadataProvider>(executor);
    }
};

TEST_F(UserMetadataProviderTest, ShouldCollectCurrentUserInfo) {
    auto data = provider->get_user_metadata();
    
    // Nao usamos mock para funcoes C (getuid, etc), entao validamos se nao estao vazios
    EXPECT_FALSE(data.currentUser.isEmpty());
    EXPECT_FALSE(data.shell.isEmpty());
    EXPECT_FALSE(data.home.isEmpty());
}

TEST_F(UserMetadataProviderTest, ShouldCallShellForLimitsAndHistory) {
    EXPECT_CALL(*executor, execute(QString("bash"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>("core file size (blocks, -c) 0\ndata seg size (kbytes, -d) unlimited"), Return(0)));
        
    EXPECT_CALL(*executor, execute(QString("last"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>("petrus   pts/0        :0               Thu Apr  9 10:00 still logged in"), Return(0)));
        
    auto data = provider->get_user_metadata();
    
    ASSERT_GE(data.systemLimits.size(), 1);
    EXPECT_TRUE(data.systemLimits[0].contains("core file size"));
    
    ASSERT_GE(data.loginHistory.size(), 1);
    EXPECT_TRUE(data.loginHistory[0].contains("petrus"));
}

TEST_F(UserMetadataProviderTest, ShouldCollectEnvVars) {
    auto data = provider->get_user_metadata();
    
    // HOME e PATH quase sempre estao definidos em ambiente de teste
    EXPECT_TRUE(data.envVars.contains("HOME"));
    EXPECT_TRUE(data.envVars.contains("PATH"));
}
