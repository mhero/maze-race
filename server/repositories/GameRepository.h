#pragma once
#include <functional>
#include <string>
#include <vector>

struct WinRecord
{
    std::string username;
    int wins;
};

// Pure data-access layer for the `games` table.
class GameRepository
{
  public:
    static GameRepository &instance();

    using InsertedCallback = std::function<void()>;
    using TopWinnersCallback = std::function<void(const std::vector<WinRecord> &winners)>;
    using ErrorCallback = std::function<void(const std::string &message)>;

    void insertWin(const std::string &roomCode, const std::string &username, InsertedCallback onResult,
                   ErrorCallback onError);
    void getTopWinners(int limit, TopWinnersCallback onResult, ErrorCallback onError);

  private:
    GameRepository() = default;
};
