#pragma once

#include <string>

class AdminConfig {
public:
    AdminConfig();
    bool loadPasswordHash(std::string& hashOut, std::string& saltOut);
    void savePasswordHash(const std::string& hash, const std::string& salt);

    static std::string hashPassword(const std::string& password, const std::string& salt);
    static std::string generateSalt(size_t length = 32);
};