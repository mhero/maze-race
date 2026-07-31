#include "JwtService.h"
#include <drogon/drogon.h>
#include <openssl/hmac.h>
#include <ctime>
#include <sstream>
#include <vector>

namespace
{
const std::string kBase64UrlChars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

std::string base64UrlEncode(const unsigned char *data, size_t len)
{
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    size_t i = 0;
    while (i + 3 <= len)
    {
        unsigned int n = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
        out += kBase64UrlChars[(n >> 18) & 0x3F];
        out += kBase64UrlChars[(n >> 12) & 0x3F];
        out += kBase64UrlChars[(n >> 6) & 0x3F];
        out += kBase64UrlChars[n & 0x3F];
        i += 3;
    }
    size_t rem = len - i;
    if (rem == 1)
    {
        unsigned int n = data[i] << 16;
        out += kBase64UrlChars[(n >> 18) & 0x3F];
        out += kBase64UrlChars[(n >> 12) & 0x3F];
    }
    else if (rem == 2)
    {
        unsigned int n = (data[i] << 16) | (data[i + 1] << 8);
        out += kBase64UrlChars[(n >> 18) & 0x3F];
        out += kBase64UrlChars[(n >> 12) & 0x3F];
        out += kBase64UrlChars[(n >> 6) & 0x3F];
    }
    return out;
}

std::string base64UrlEncode(const std::string &s)
{
    return base64UrlEncode(reinterpret_cast<const unsigned char *>(s.data()), s.size());
}

std::vector<unsigned char> base64UrlDecode(const std::string &input)
{
    auto valueOf = [](char c) -> int {
        auto pos = kBase64UrlChars.find(c);
        return pos == std::string::npos ? -1 : static_cast<int>(pos);
    };

    std::vector<unsigned char> out;
    int bitBuffer = 0, bitCount = 0;
    for (char c : input)
    {
        int v = valueOf(c);
        if (v < 0)
            continue;
        bitBuffer = (bitBuffer << 6) | v;
        bitCount += 6;
        if (bitCount >= 8)
        {
            bitCount -= 8;
            out.push_back(static_cast<unsigned char>((bitBuffer >> bitCount) & 0xFF));
        }
    }
    return out;
}

std::vector<unsigned char> hmacSha256(const std::string &key, const std::string &data)
{
    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    HMAC(EVP_sha256(), key.data(), static_cast<int>(key.size()),
         reinterpret_cast<const unsigned char *>(data.data()), data.size(), result, &len);
    return std::vector<unsigned char>(result, result + len);
}

std::vector<std::string> splitToken(const std::string &token)
{
    std::vector<std::string> parts;
    std::stringstream ss(token);
    std::string part;
    while (std::getline(ss, part, '.'))
        parts.push_back(part);
    return parts;
}
}  // namespace

JwtService &JwtService::instance()
{
    static JwtService svc;
    return svc;
}

std::string JwtService::secret() const
{
    return drogon::app().getCustomConfig()["jwt_secret"].asString();
}

int JwtService::expirySeconds() const
{
    auto v = drogon::app().getCustomConfig()["jwt_expiry_seconds"];
    return v.isNull() ? 86400 : v.asInt();
}

std::string JwtService::sign(const std::string &username) const
{
    Json::Value header;
    header["alg"] = "HS256";
    header["typ"] = "JWT";

    Json::Value payload;
    payload["sub"] = username;
    auto now = static_cast<Json::Int64>(std::time(nullptr));
    payload["iat"] = now;
    payload["exp"] = now + expirySeconds();

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";

    std::string headerB64 = base64UrlEncode(Json::writeString(builder, header));
    std::string payloadB64 = base64UrlEncode(Json::writeString(builder, payload));
    std::string signingInput = headerB64 + "." + payloadB64;

    auto sig = hmacSha256(secret(), signingInput);
    std::string sigB64 = base64UrlEncode(sig.data(), sig.size());

    return signingInput + "." + sigB64;
}

std::optional<std::string> JwtService::verify(const std::string &token) const
{
    auto parts = splitToken(token);
    if (parts.size() != 3)
        return std::nullopt;

    std::string signingInput = parts[0] + "." + parts[1];
    auto expectedSig = hmacSha256(secret(), signingInput);
    std::string expectedSigB64 = base64UrlEncode(expectedSig.data(), expectedSig.size());

    if (expectedSigB64 != parts[2])
        return std::nullopt;

    auto payloadBytes = base64UrlDecode(parts[1]);
    std::string payloadStr(payloadBytes.begin(), payloadBytes.end());

    Json::CharReaderBuilder readerBuilder;
    Json::Value payload;
    std::string errs;
    std::istringstream iss(payloadStr);
    if (!Json::parseFromStream(readerBuilder, iss, &payload, &errs))
        return std::nullopt;

    if (!payload.isMember("sub") || !payload.isMember("exp"))
        return std::nullopt;

    auto exp = payload["exp"].asInt64();
    auto now = static_cast<Json::Int64>(std::time(nullptr));
    if (now >= exp)
        return std::nullopt;

    return payload["sub"].asString();
}
