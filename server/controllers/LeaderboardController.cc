#include "LeaderboardController.h"
#include "../services/LeaderboardService.h"
#include <drogon/drogon.h>

using namespace drogon;

void LeaderboardController::getLeaderboard(const HttpRequestPtr &req,
                                            std::function<void(const HttpResponsePtr &)> &&callback)
{
    LeaderboardService::getTop(
        10,
        [callback](const Json::Value &arr) { callback(HttpResponse::newHttpJsonResponse(arr)); },
        [callback](const std::string &message) {
            Json::Value err;
            err["error"] = message;
            auto resp = HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        });
}
