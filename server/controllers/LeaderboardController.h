#pragma once
#include <drogon/HttpController.h>

class LeaderboardController : public drogon::HttpController<LeaderboardController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(LeaderboardController::getLeaderboard, "/api/leaderboard", drogon::Get);
    METHOD_LIST_END

    void getLeaderboard(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};
