#pragma once
#include <drogon/WebSocketConnection.h>
#include <json/json.h>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// Context stashed on each WebSocket connection so we know who they are and
// which room they belong to on every subsequent message / disconnect.
struct WsPlayerContext
{
    std::string username;
    std::string room;
};

struct PlayerState
{
    std::string username;
    int row = 0;
    int col = 0;
    std::string color;
    drogon::WebSocketConnectionPtr conn;
};

struct GameRoom
{
    std::string code;
    int size = 15;
    std::vector<uint8_t> walls;
    std::map<std::string, PlayerState> players;
    bool finished = false;
    std::string winner;
};

enum class MoveResult
{
    INVALID_MOVE,
    MOVED,
    WON,
    ROOM_FINISHED,
    NOT_FOUND
};

// Thread-safe, in-memory registry of active game rooms. Rooms live only for
// the lifetime of the server process (fine for a showcase project).
class GameRoomManager
{
  public:
    static GameRoomManager &instance();

    std::shared_ptr<GameRoom> createRoom(int size);
    std::shared_ptr<GameRoom> getRoom(const std::string &code);

    bool joinRoom(const std::string &code, const std::string &username,
                  const drogon::WebSocketConnectionPtr &conn, int &outRow, int &outCol,
                  std::string &outColor);

    void leaveRoom(const std::string &code, const std::string &username);

    MoveResult tryMove(const std::string &code, const std::string &username, const std::string &dir,
                        int &newRow, int &newCol);

    void broadcast(const std::string &code, const Json::Value &msg, const std::string &exceptUsername = "");

    Json::Value playersJson(const std::string &code);

  private:
    GameRoomManager() = default;
    std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<GameRoom>> rooms_;
};
