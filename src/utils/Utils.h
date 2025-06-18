#pragma once
#include <string>

class Utils {
public:
    static std::string trim(const std::string& str);
    static std::string unescape(const std::string& input);
    static std::string base64Encode(const std::string& input);
    static std::string base64Decode(const std::string& input);
    static std::string generateRandomSalt(size_t length = 32);
};
