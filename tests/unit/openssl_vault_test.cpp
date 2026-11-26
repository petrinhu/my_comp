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

// CONTRACT §8.3: VaultData vem de disco (entrada externa). O OpenSSL lê exatamente
// 12 bytes de IV e 16 de tag; tamanhos menores causavam leitura fora do buffer.
TEST(OpenSslVaultTest, DecryptRejectsShortOrLongIv) {
    OpenSslVault vault;
    std::vector<uint8_t> key(32, 0x07);
    auto data = vault.encrypt({1, 2, 3, 4}, key);

    auto short_iv = data;
    short_iv.iv.resize(4);
    EXPECT_TRUE(vault.decrypt(short_iv, key).empty());

    auto empty_iv = data;
    empty_iv.iv.clear();
    EXPECT_TRUE(vault.decrypt(empty_iv, key).empty());

    auto long_iv = data;
    long_iv.iv.push_back(0x00);
    EXPECT_TRUE(vault.decrypt(long_iv, key).empty());
}

TEST(OpenSslVaultTest, DecryptRejectsShortTag) {
    OpenSslVault vault;
    std::vector<uint8_t> key(32, 0x07);
    auto data = vault.encrypt({1, 2, 3, 4}, key);

    auto short_tag = data;
    short_tag.tag.resize(8);
    EXPECT_TRUE(vault.decrypt(short_tag, key).empty());

    auto empty_tag = data;
    empty_tag.tag.clear();
    EXPECT_TRUE(vault.decrypt(empty_tag, key).empty());

    // Red determinístico: com 17 bytes o OpenSSL usa os 16 primeiros e decifra com sucesso.
    auto long_tag = data;
    long_tag.tag.push_back(0x00);
    EXPECT_TRUE(vault.decrypt(long_tag, key).empty());
}
