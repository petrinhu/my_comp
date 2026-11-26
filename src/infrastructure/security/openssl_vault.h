#pragma once
#include "../../domain/interfaces/i_security_vault.h"
#include <openssl/evp.h>
#include <openssl/rand.h>

namespace my_comp::infrastructure::security {

class OpenSslVault : public domain::interfaces::ISecurityVault {
public:
    domain::models::VaultData encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& key) override;
    std::vector<uint8_t> decrypt(const domain::models::VaultData& data, const std::vector<uint8_t>& key) override;

private:
    static constexpr std::size_t AES_KEY_SIZE = 32;
    static constexpr std::size_t GCM_IV_SIZE = 12;
    static constexpr std::size_t GCM_TAG_SIZE = 16;
};

} // namespace my_comp::infrastructure::security
