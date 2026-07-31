#include "AuthController.h"
#include "../services/JwtService.h"
#include "../services/PasswordService.h"
#include <drogon/drogon.h>

using namespace drogon;

namespace
{
HttpResponsePtr errorResponse(const std::string &message, HttpStatusCode code)
{
    Json::Value err;
    err["error"] = message;
    auto resp = HttpResponse::newHttpJsonResponse(err);
    resp->setStatusCode(code);
    return resp;
}
}  // namespace

void AuthController::registerUser(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("username") || !json->isMember("password"))
    {
        callback(errorResponse("username and password are required", k400BadRequest));
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    if (username.size() < 3 || username.size() > 24 || password.size() < 4)
    {
        callback(errorResponse("username must be 3-24 chars and password >= 4 chars", k400BadRequest));
        return;
    }

    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT id FROM users WHERE username = ?",
        [db, username, password, callback](const orm::Result &r) mutable {
            if (r.size() > 0)
            {
                callback(errorResponse("username already taken", k409Conflict));
                return;
            }

            std::string salt, hash;
            PasswordService::hashPassword(password, salt, hash);

            db->execSqlAsync(
                "INSERT INTO users (username, salt, password_hash, created_at) VALUES (?, ?, ?, datetime('now'))",
                [callback, username](const orm::Result &) {
                    auto token = JwtService::instance().sign(username);
                    Json::Value res;
                    res["token"] = token;
                    res["username"] = username;
                    callback(HttpResponse::newHttpJsonResponse(res));
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(errorResponse(std::string("db error: ") + e.base().what(), k500InternalServerError));
                },
                username, salt, hash);
        },
        [callback](const orm::DrogonDbException &e) {
            callback(errorResponse(std::string("db error: ") + e.base().what(), k500InternalServerError));
        },
        username);
}

void AuthController::login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("username") || !json->isMember("password"))
    {
        callback(errorResponse("username and password are required", k400BadRequest));
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT salt, password_hash FROM users WHERE username = ?",
        [callback, username, password](const orm::Result &r) {
            if (r.size() == 0)
            {
                callback(errorResponse("invalid username or password", k401Unauthorized));
                return;
            }

            std::string salt = r[0]["salt"].as<std::string>();
            std::string hash = r[0]["password_hash"].as<std::string>();

            if (!PasswordService::verifyPassword(password, salt, hash))
            {
                callback(errorResponse("invalid username or password", k401Unauthorized));
                return;
            }

            auto token = JwtService::instance().sign(username);
            Json::Value res;
            res["token"] = token;
            res["username"] = username;
            callback(HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(errorResponse(std::string("db error: ") + e.base().what(), k500InternalServerError));
        },
        username);
}
