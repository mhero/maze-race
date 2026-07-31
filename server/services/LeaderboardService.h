#pragma once
#include <string>

class LeaderboardService
{
  public:
    // Fire-and-forget insert of a completed game's result.
    static void recordWin(const std::string &roomCode, const std::string &username);
};
