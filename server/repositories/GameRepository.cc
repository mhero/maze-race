#include "GameRepository.h"
#include <drogon/drogon.h>

using namespace drogon;

GameRepository &GameRepository::instance()
{
    static GameRepository repo;
    return repo;
}

void GameRepository::insertWin(const std::string &roomCode, const std::string &username, InsertedCallback onResult,
                                ErrorCallback onError)
{
    app().getDbClient()->execSqlAsync(
        "INSERT INTO games (room_code, winner, created_at) VALUES (?, ?, datetime('now'))",
        [onResult](const orm::Result &) { onResult(); },
        [onError](const orm::DrogonDbException &e) { onError(std::string("db error: ") + e.base().what()); },
        roomCode, username);
}

void GameRepository::getTopWinners(int limit, TopWinnersCallback onResult, ErrorCallback onError)
{
    app().getDbClient()->execSqlAsync(
        "SELECT winner, COUNT(*) as wins FROM games GROUP BY winner ORDER BY wins DESC LIMIT ?",
        [onResult](const orm::Result &r) {
            std::vector<WinRecord> winners;
            winners.reserve(r.size());
            for (auto row : r)
                winners.push_back(WinRecord{row["winner"].as<std::string>(), row["wins"].as<int>()});
            onResult(winners);
        },
        [onError](const orm::DrogonDbException &e) { onError(std::string("db error: ") + e.base().what()); },
        limit);
}
