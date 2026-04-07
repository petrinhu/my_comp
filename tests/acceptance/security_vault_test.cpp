#include <gtest/gtest.h>
#include "../../src/infrastructure/security/openssl_vault.h"
#include <vector>

using namespace my_comp::infrastructure::security;

TEST(SecurityVaultAcceptanceTest, EncryptDecryptGCM_IntegrityCheck) {
    OpenSslVault vault;
    
    // Chave de 32 bytes (AES-256)
    std::vector<uint8_t> key(32, 0x42);
    std::string secret = "SensitiveData123";
    std::vector<uint8_t> plaintext(secret.begin(), secret.end());

    // Criptografa
    auto encrypted = vault.encrypt(plaintext, key);
    
    EXPECT_FALSE(encrypted.ciphertext.empty());
    EXPECT_FALSE(encrypted.iv.empty());
    EXPECT_FALSE(encrypted.tag.empty());

    // Descriptografa
    auto decrypted = vault.decrypt(encrypted, key);

    EXPECT_EQ(decrypted, plaintext);

    // Teste de integridade (alterar um bit do ciphertext deve falhar)
    encrypted.ciphertext[0] ^= 0x01;
    auto corrupted = vault.decrypt(encrypted, key);
    EXPECT_TRUE(corrupted.empty());
}
