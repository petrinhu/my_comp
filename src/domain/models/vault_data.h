#pragma once
#include <vector>
#include <cstdint>

namespace my_comp::domain::models {

struct VaultData {
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> iv;
    std::vector<uint8_t> tag;
};

} // namespace my_comp::domain::models
