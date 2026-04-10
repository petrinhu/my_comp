#pragma once
#include "../models/user_metadata.h"

namespace my_comp::domain::interfaces {

class IUserMetadataProvider {
public:
    virtual ~IUserMetadataProvider() = default;
    virtual models::UserMetadata get_user_metadata() = 0;
};

} // namespace my_comp::domain::interfaces
