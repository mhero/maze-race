#pragma once
#include <functional>
#include <string>

// Owns every SQL statement touching the `users` table. Controllers should
// never see raw SQL — they just call these and translate the outcome into
// an HTTP response.
class UserService
{
  public:
    // status: 200 on success, 400/401/409/500 on failure (maps directly onto
    // an HttpStatusCode in the controller). On success, token/message carries
    // the JWT; on failure it carries a human-readable error message.
    using ResultCallback = std::function<void(int status, const std::string &tokenOrMessage)>;

    static void registerUser(const std::string &username, const std::string &password, ResultCallback callback);
    static void login(const std::string &username, const std::string &password, ResultCallback callback);
};
