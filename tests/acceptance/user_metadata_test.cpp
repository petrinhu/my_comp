#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../../src/domain/interfaces/i_user_metadata_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

class MockUserMetadataProvider : public IUserMetadataProvider {
public:
    MOCK_METHOD(UserMetadata, get_user_metadata, (), (override));
};

TEST(UserMetadataAcceptance, ShouldCollectUserDataContract) {
    auto provider = std::make_unique<MockUserMetadataProvider>();
    
    UserMetadata fake_data;
    fake_data.currentUser = "petrus";
    fake_data.shell = "/bin/bash";
    fake_data.envVars["HOME"] = "/home/petrus";
    
    EXPECT_CALL(*provider, get_user_metadata()).WillOnce(testing::Return(fake_data));
    
    auto data = provider->get_user_metadata();
    EXPECT_EQ(data.currentUser, "petrus");
    EXPECT_EQ(data.shell, "/bin/bash");
    EXPECT_EQ(data.envVars["HOME"], "/home/petrus");
}
