#include "UserRepository.h"
#include <drogon/drogon.h>

using namespace drogon;

UserRepository &UserRepository::instance()
{
    static UserRepository repo;
    return repo;
}

void UserRepository::existsByUsername(const std::string &username, ExistsCallback onResult, ErrorCallback onError)
{
    app().getDbClient()->execSqlAsync(
        "SELECT id FROM users WHERE username = ?",
        [onResult](const orm::Result &r) { onResult(r.size() > 0); },
        [onError](const orm::DrogonDbException &e) { onError(std::string("db error: ") + e.base().what()); },
        username);
}

void UserRepository::findByUsername(const std::string &username, FoundCallback onResult, ErrorCallback onError)
{
    app().getDbClient()->execSqlAsync(
        "SELECT username, salt, password_hash FROM users WHERE username = ?",
        [onResult](const orm::Result &r) {
            if (r.size() == 0)
            {
                onResult(std::nullopt);
                return;
            }
            UserRecord rec;
            rec.username = r[0]["username"].as<std::string>();
            rec.salt = r[0]["salt"].as<std::string>();
            rec.passwordHash = r[0]["password_hash"].as<std::string>();
            onResult(rec);
        },
        [onError](const orm::DrogonDbException &e) { onError(std::string("db error: ") + e.base().what()); },
        username);
}

void UserRepository::insert(const std::string &username, const std::string &salt, const std::string &passwordHash,
                             InsertedCallback onResult, ErrorCallback onError)
{
    app().getDbClient()->execSqlAsync(
        "INSERT INTO users (username, salt, password_hash, created_at) VALUES (?, ?, ?, datetime('now'))",
        [onResult](const orm::Result &) { onResult(); },
        [onError](const orm::DrogonDbException &e) { onError(std::string("db error: ") + e.base().what()); },
        username, salt, passwordHash);
}
