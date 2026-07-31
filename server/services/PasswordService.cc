#include "PasswordService.h"
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

namespace
{
std::string toHex(const unsigned char *data, size_t len)
{
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    return oss.str();
}

std::string sha256Hex(const std::string &input)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(input.data()), input.size(), hash);
    return toHex(hash, SHA256_DIGEST_LENGTH);
}
}  // namespace

void PasswordService::hashPassword(const std::string &password, std::string &saltHexOut, std::string &hashHexOut)
{
    unsigned char salt[16];
    RAND_bytes(salt, sizeof(salt));
    saltHexOut = toHex(salt, sizeof(salt));
    hashHexOut = sha256Hex(saltHexOut + password);
}

bool PasswordService::verifyPassword(const std::string &password, const std::string &saltHex, const std::string &hashHex)
{
    return sha256Hex(saltHex + password) == hashHex;
}
