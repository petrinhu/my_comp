#include <gtest/gtest.h>
#include "../../src/infrastructure/security/openssl_vault.h"
#include <string>

using namespace my_comp::infrastructure::security;
using namespace my_comp::domain::models;

TEST(OpenSslVaultTest, ShouldEncryptAndDecryptData) {
    OpenSslVault vault;
    std::string secret = "Sensitive system configuration data";
    std::vector<uint8_t> plaintext(secret.begin(), secret.end());
    std::vector<uint8_t> key(32, 0x42); // 256-bit key
    
    auto vaultData = vault.encrypt(plaintext, key);
    
    EXPECT_FALSE(vaultData.ciphertext.empty());
    EXPECT_EQ(vaultData.iv.size(), 12);
    EXPECT_EQ(vaultData.tag.size(), 16);
    
    auto decrypted = vault.decrypt(vaultData, key);
    std::string recovered(decrypted.begin(), decrypted.end());
    
    EXPECT_EQ(secret, recovered);
}

TEST(OpenSslVaultTest, ShouldFailWithWrongKey) {
    OpenSslVault vault;
    std::vector<uint8_t> plaintext = {1, 2, 3, 4};
    std::vector<uint8_t> key1(32, 0x01);
    std::vector<uint8_t> key2(32, 0x02);
    
    auto vaultData = vault.encrypt(plaintext, key1);
    
    // Decrypt com chave errada deve falhar (retornar vetor vazio ou lançar erro, dependendo da implementação)
    // No caso do GCM, o OpenSSL detecta falha na tag.
    auto decrypted = vault.decrypt(vaultData, key2);
    EXPECT_TRUE(decrypted.empty());
}

TEST(OpenSslVaultTest, ShouldFailIfDataTampered) {
    OpenSslVault vault;
    std::vector<uint8_t> plaintext = {1, 2, 3, 4};
    std::vector<uint8_t> key(32, 0x01);

    auto vaultData = vault.encrypt(plaintext, key);

    // Tamper com o ciphertext
    vaultData.ciphertext[0] ^= 0xFF;

    auto decrypted = vault.decrypt(vaultData, key);
    EXPECT_TRUE(decrypted.empty());
}

TEST(OpenSslVaultTest, EncryptShouldThrowOnWrongKeySize) {
    OpenSslVault vault;
    std::vector<uint8_t> plaintext = {1, 2, 3, 4};

    EXPECT_THROW(vault.encrypt(plaintext, std::vector<uint8_t>(16, 0x01)), std::invalid_argument);
    EXPECT_THROW(vault.encrypt(plaintext, std::vector<uint8_t>(31, 0x01)), std::invalid_argument);
    EXPECT_THROW(vault.encrypt(plaintext, std::vector<uint8_t>(33, 0x01)), std::invalid_argument);
    EXPECT_THROW(vault.encrypt(plaintext, std::vector<uint8_t>{}), std::invalid_argument);
}

TEST(OpenSslVaultTest, DecryptShouldReturnEmptyOnWrongKeySize) {
    OpenSslVault vault;
    std::vector<uint8_t> plaintext = {1, 2, 3, 4};
    std::vector<uint8_t> key(32, 0x01);

    auto vaultData = vault.encrypt(plaintext, key);

    EXPECT_TRUE(vault.decrypt(vaultData, std::vector<uint8_t>(16, 0x01)).empty());
    EXPECT_TRUE(vault.decrypt(vaultData, std::vector<uint8_t>(31, 0x01)).empty());
    EXPECT_TRUE(vault.decrypt(vaultData, std::vector<uint8_t>{}).empty());
}

TEST(OpenSslVaultTest, ShouldFailIfIvTampered) {
    OpenSslVault vault;
    std::vector<uint8_t> plaintext = {1, 2, 3, 4};
    std::vector<uint8_t> key(32, 0x01);

    auto vaultData = vault.encrypt(plaintext, key);
    ASSERT_FALSE(vaultData.iv.empty());

    vaultData.iv[0] ^= 0xFF;

    auto decrypted = vault.decrypt(vaultData, key);
    EXPECT_TRUE(decrypted.empty());
}

TEST(OpenSslVaultTest, ShouldFailIfTagTampered) {
    OpenSslVault vault;
    std::vector<uint8_t> plaintext = {1, 2, 3, 4};
    std::vector<uint8_t> key(32, 0x01);

    auto vaultData = vault.encrypt(plaintext, key);
    ASSERT_FALSE(vaultData.tag.empty());

    vaultData.tag[0] ^= 0xFF;

    auto decrypted = vault.decrypt(vaultData, key);
    EXPECT_TRUE(decrypted.empty());
}

TEST(OpenSslVaultTest, ShouldRoundTripEmptyPlaintext) {
    OpenSslVault vault;
    std::vector<uint8_t> plaintext;
    std::vector<uint8_t> key(32, 0x42);

    auto vaultData = vault.encrypt(plaintext, key);

    EXPECT_TRUE(vaultData.ciphertext.empty());
    EXPECT_EQ(vaultData.iv.size(), 12u);
    EXPECT_EQ(vaultData.tag.size(), 16u);

    auto decrypted = vault.decrypt(vaultData, key);
    EXPECT_TRUE(decrypted.empty());

    // Tag autentica "mensagem vazia": violar a tag deve quebrar a verificação mesmo sem ciphertext
    vaultData.tag[0] ^= 0xFF;
    EXPECT_TRUE(vault.decrypt(vaultData, key).empty());
}
