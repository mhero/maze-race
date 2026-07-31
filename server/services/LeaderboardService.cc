#include "LeaderboardService.h"
#include <drogon/drogon.h>

using namespace drogon;

void LeaderboardService::recordWin(const std::string &roomCode, const std::string &username)
{
    auto db = app().getDbClient();
    db->execSqlAsync(
        "INSERT INTO games (room_code, winner, created_at) VALUES (?, ?, datetime('now'))",
        [](const orm::Result &) { /* no-op */ },
        [](const orm::DrogonDbException &e) { LOG_ERROR << "Failed to record win: " << e.base().what(); },
        roomCode, username);
}
