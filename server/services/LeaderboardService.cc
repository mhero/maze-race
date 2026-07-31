#include "LeaderboardService.h"
#include "../repositories/GameRepository.h"
#include <drogon/drogon.h>

void LeaderboardService::recordWin(const std::string &roomCode, const std::string &username)
{
    GameRepository::instance().insertWin(
        roomCode, username, []() { /* no-op */ },
        [](const std::string &err) { LOG_ERROR << "Failed to record win: " << err; });
}

void LeaderboardService::getTop(int limit, ResultCallback onResult, ErrorCallback onError)
{
    GameRepository::instance().getTopWinners(
        limit,
        [onResult](const std::vector<WinRecord> &winners) {
            Json::Value arr(Json::arrayValue);
            for (const auto &w : winners)
            {
                Json::Value item;
                item["username"] = w.username;
                item["wins"] = w.wins;
                arr.append(item);
            }
            onResult(arr);
        },
        onError);
}
