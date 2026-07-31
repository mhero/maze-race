#pragma once
#include <json/json.h>
#include <functional>
#include <string>

class LeaderboardService
{
  public:
    // Fire-and-forget insert of a completed game's result.
    static void recordWin(const std::string &roomCode, const std::string &username);

    // Top winners by win count, as a Json array of {username, wins}.
    using ResultCallback = std::function<void(const Json::Value &leaderboard)>;
    using ErrorCallback = std::function<void(const std::string &message)>;
    static void getTop(int limit, ResultCallback onResult, ErrorCallback onError);
};
