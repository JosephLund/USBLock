#include "AdminConfig.h"
#include "../utils/Utils.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <filesystem>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

AdminConfig::AdminConfig() {
    std::filesystem::create_directories("config");
}

bool AdminConfig::loadPasswordHash(std::string& hashOut, std::string& saltOut) {
    std::ifstream file("config/admin.cfg");
    if (!file.is_open())
        return false;

    std::getline(file, saltOut);
    std::getline(file, hashOut);
    file.close();
    return true;
}

void AdminConfig::savePasswordHash(const std::string& hash, const std::string& salt) {
    std::ofstream file("config/admin.cfg");
    file << salt << "\n" << hash;
    file.close();
}

std::string AdminConfig::generateSalt(size_t length) {
    const char charset[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string salt;
    for (size_t i = 0; i < length; ++i)
        salt += charset[dist(gen)];

    return salt;
}

std::string AdminConfig::hashPassword(const std::string& password, const std::string& salt) {
    std::string salted = password + salt;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(salted.c_str()), salted.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return ss.str();
}

std::string AdminConfig::encryptString(const std::string& plaintext, const std::string& key) {
    unsigned char iv[16];
    RAND_bytes(iv, sizeof(iv));

    unsigned char keyHash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(key.c_str()), key.length(), keyHash);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::string encrypted;
    int len;
    int ciphertext_len;
    unsigned char ciphertext[plaintext.size() + AES_BLOCK_SIZE];

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, keyHash, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    std::string result(reinterpret_cast<char*>(iv), sizeof(iv));
    result.append(reinterpret_cast<char*>(ciphertext), ciphertext_len);
    return Utils::base64Encode(result);
}

std::string AdminConfig::decryptString(const std::string& encoded, const std::string& key) {
    std::string decoded = Utils::base64Decode(encoded);
    if (decoded.size() < 16) return "";

    const unsigned char* iv = reinterpret_cast<const unsigned char*>(decoded.data());
    const unsigned char* ciphertext = reinterpret_cast<const unsigned char*>(decoded.data() + 16);
    int ciphertext_len = decoded.size() - 16;

    unsigned char keyHash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(key.c_str()), key.length(), keyHash);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    unsigned char plaintext[ciphertext_len + AES_BLOCK_SIZE];
    int len;
    int plaintext_len;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, keyHash, iv);
    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    return std::string(reinterpret_cast<char*>(plaintext), plaintext_len);
}
