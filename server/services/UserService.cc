#include "UserService.h"
#include "../repositories/UserRepository.h"
#include "JwtService.h"
#include "PasswordService.h"

void UserService::registerUser(const std::string &username, const std::string &password, ResultCallback callback)
{
    if (username.size() < 3 || username.size() > 24 || password.size() < 4)
    {
        callback(400, "username must be 3-24 chars and password >= 4 chars");
        return;
    }

    UserRepository::instance().existsByUsername(
        username,
        [username, password, callback](bool exists) {
            if (exists)
            {
                callback(409, "username already taken");
                return;
            }

            std::string salt, hash;
            PasswordService::hashPassword(password, salt, hash);

            UserRepository::instance().insert(
                username, salt, hash,
                [username, callback]() { callback(200, JwtService::instance().sign(username)); },
                [callback](const std::string &err) { callback(500, err); });
        },
        [callback](const std::string &err) { callback(500, err); });
}

void UserService::login(const std::string &username, const std::string &password, ResultCallback callback)
{
    UserRepository::instance().findByUsername(
        username,
        [username, password, callback](const std::optional<UserRecord> &user) {
            if (!user || !PasswordService::verifyPassword(password, user->salt, user->passwordHash))
            {
                callback(401, "invalid username or password");
                return;
            }
            callback(200, JwtService::instance().sign(username));
        },
        [callback](const std::string &err) { callback(500, err); });
}
