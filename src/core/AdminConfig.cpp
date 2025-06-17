#include "AdminConfig.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <filesystem>
#include <openssl/sha.h>

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
