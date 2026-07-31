#pragma once
#include <string>

// NOTE: this is a deliberately lightweight salted-SHA256 password hash meant
// to keep the showcase project dependency-free (OpenSSL only). For a real
// production system, use a slow adaptive hash such as bcrypt or argon2.
class PasswordService
{
  public:
    static void hashPassword(const std::string &password, std::string &saltHexOut, std::string &hashHexOut);
    static bool verifyPassword(const std::string &password, const std::string &saltHex, const std::string &hashHex);
};
