#pragma once
#include "../models/disk_inventory.h"

namespace my_comp::domain::interfaces {

class IDiskInventoryProvider {
public:
    virtual ~IDiskInventoryProvider() = default;

    virtual models::DiskInventory collect_inventory() = 0;
};

} // namespace my_comp::domain::interfaces
