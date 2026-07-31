#include "AuthController.h"
#include "../services/UserService.h"
#include <drogon/drogon.h>

using namespace drogon;

namespace
{
void respond(std::function<void(const HttpResponsePtr &)> &callback, int status, const std::string &tokenOrMessage,
             const std::string &username)
{
    if (status == 200)
    {
        Json::Value res;
        res["token"] = tokenOrMessage;
        res["username"] = username;
        callback(HttpResponse::newHttpJsonResponse(res));
        return;
    }

    Json::Value err;
    err["error"] = tokenOrMessage;
    auto resp = HttpResponse::newHttpJsonResponse(err);
    resp->setStatusCode(static_cast<HttpStatusCode>(status));
    callback(resp);
}
}  // namespace

void AuthController::registerUser(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("username") || !json->isMember("password"))
    {
        respond(callback, 400, "username and password are required", "");
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    UserService::registerUser(username, password,
                               [callback = std::move(callback), username](int status, const std::string &msg) mutable {
                                   respond(callback, status, msg, username);
                               });
}

void AuthController::login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("username") || !json->isMember("password"))
    {
        respond(callback, 400, "username and password are required", "");
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    UserService::login(username, password,
                        [callback = std::move(callback), username](int status, const std::string &msg) mutable {
                            respond(callback, status, msg, username);
                        });
}
