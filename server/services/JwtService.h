#pragma once
#include <optional>
#include <string>

// Minimal, dependency-free HS256 JWT signer/verifier (OpenSSL HMAC-SHA256
// under the hood). Good enough for a showcase project; for production use a
// battle-tested JWT library.
class JwtService
{
  public:
    static JwtService &instance();

    std::string sign(const std::string &username) const;
    std::optional<std::string> verify(const std::string &token) const;

  private:
    JwtService() = default;
    std::string secret() const;
    int expirySeconds() const;
};
