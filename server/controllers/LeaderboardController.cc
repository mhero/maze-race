#include "LeaderboardController.h"
#include <drogon/drogon.h>

using namespace drogon;

void LeaderboardController::getLeaderboard(const HttpRequestPtr &req,
                                            std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT winner, COUNT(*) as wins FROM games GROUP BY winner ORDER BY wins DESC LIMIT 10",
        [callback](const orm::Result &r) {
            Json::Value arr(Json::arrayValue);
            for (auto row : r)
            {
                Json::Value item;
                item["username"] = row["winner"].as<std::string>();
                item["wins"] = row["wins"].as<int>();
                arr.append(item);
            }
            callback(HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const orm::DrogonDbException &e) {
            Json::Value err;
            err["error"] = std::string("db error: ") + e.base().what();
            auto resp = HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        });
}
