#include "Utils.h"
#include <algorithm>
#include <cctype>
#include <random>
#include <sstream>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string Utils::unescape(const std::string& input) {
    std::string output = input;
    size_t pos = 0;
    while ((pos = output.find("&amp;", pos)) != std::string::npos) {
        output.replace(pos, 5, "&");
        pos += 1;
    }
    return output;
}
std::string Utils::base64Encode(const std::string& input) {
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bio);

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // No newlines
    BIO_write(b64, input.c_str(), input.length());
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bufferPtr);

    std::string output(bufferPtr->data, bufferPtr->length);
    BIO_free_all(b64);
    return output;
}

std::string Utils::base64Decode(const std::string& input) {
    BIO* bio, * b64;
    int length = input.length();
    std::string output(length, '\0');

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(input.data(), length);
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    int decodedSize = BIO_read(bio, &output[0], length);
    output.resize(decodedSize);
    BIO_free_all(bio);
    return output;
}

std::string Utils::generateRandomSalt(size_t length) {
    const char charset[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string salt;
    for (size_t i = 0; i < length; ++i) {
        salt += charset[dist(generator)];
    }

    return salt;
}