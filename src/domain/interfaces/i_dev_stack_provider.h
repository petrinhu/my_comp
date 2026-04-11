#pragma once
#include "../models/dev_stack_info.h"

namespace my_comp::domain::interfaces {

class IDevStackProvider {
public:
    virtual ~IDevStackProvider() = default;
    virtual models::DevStackInfo collect_dev_stacks() = 0;
};

} // namespace my_comp::domain::interfaces
