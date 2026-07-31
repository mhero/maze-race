#pragma once
#include <functional>
#include <optional>
#include <string>

struct UserRecord
{
    std::string username;
    std::string salt;
    std::string passwordHash;
};

// Pure data-access layer for the `users` table. No password hashing, no
// JWTs, no validation rules — just SQL in, structured results out. This is
// the only place in the codebase that should know the users table's schema.
class UserRepository
{
  public:
    static UserRepository &instance();

    using ExistsCallback = std::function<void(bool exists)>;
    using FoundCallback = std::function<void(const std::optional<UserRecord> &user)>;
    using InsertedCallback = std::function<void()>;
    using ErrorCallback = std::function<void(const std::string &message)>;

    void existsByUsername(const std::string &username, ExistsCallback onResult, ErrorCallback onError);
    void findByUsername(const std::string &username, FoundCallback onResult, ErrorCallback onError);
    void insert(const std::string &username, const std::string &salt, const std::string &passwordHash,
                InsertedCallback onResult, ErrorCallback onError);

  private:
    UserRepository() = default;
};
