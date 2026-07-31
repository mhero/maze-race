#include "GameRoomManager.h"
#include "MazeGenerator.h"
#include <random>

namespace
{
std::string generateRoomCode()
{
    static const char charset[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";  // no ambiguous chars
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);
    std::string code;
    for (int i = 0; i < 6; ++i)
        code += charset[dist(rng)];
    return code;
}

const std::vector<std::string> kColors = {"#ef4444", "#3b82f6", "#22c55e", "#eab308"};
}  // namespace

GameRoomManager &GameRoomManager::instance()
{
    static GameRoomManager mgr;
    return mgr;
}

std::shared_ptr<GameRoom> GameRoomManager::createRoom(int size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto room = std::make_shared<GameRoom>();
    std::string code;
    do
    {
        code = generateRoomCode();
    } while (rooms_.count(code));

    room->code = code;
    room->size = size;
    room->walls = maze::generate(size);
    rooms_[code] = room;
    return room;
}

std::shared_ptr<GameRoom> GameRoomManager::getRoom(const std::string &code)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rooms_.find(code);
    if (it == rooms_.end())
        return nullptr;
    return it->second;
}

bool GameRoomManager::joinRoom(const std::string &code, const std::string &username,
                                const drogon::WebSocketConnectionPtr &conn, int &outRow, int &outCol,
                                std::string &outColor)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rooms_.find(code);
    if (it == rooms_.end())
        return false;
    auto &room = it->second;
    if (room->finished)
        return false;
    if (room->players.count(username))
        return false;  // already joined from another tab
    if (room->players.size() >= 4)
        return false;

    static const std::vector<std::pair<int, int>> corners = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    int idx = static_cast<int>(room->players.size());
    auto [rowFlag, colFlag] = corners[idx];
    int row = rowFlag == 0 ? 0 : room->size - 1;
    int col = colFlag == 0 ? 0 : room->size - 1;
    std::string color = kColors[idx % kColors.size()];

    PlayerState state;
    state.username = username;
    state.row = row;
    state.col = col;
    state.color = color;
    state.conn = conn;
    room->players[username] = state;

    outRow = row;
    outCol = col;
    outColor = color;
    return true;
}

void GameRoomManager::leaveRoom(const std::string &code, const std::string &username)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rooms_.find(code);
    if (it == rooms_.end())
        return;
    it->second->players.erase(username);
}

MoveResult GameRoomManager::tryMove(const std::string &code, const std::string &username, const std::string &dir,
                                     int &newRow, int &newCol)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rooms_.find(code);
    if (it == rooms_.end())
        return MoveResult::NOT_FOUND;
    auto &room = it->second;
    if (room->finished)
        return MoveResult::ROOM_FINISHED;

    auto pit = room->players.find(username);
    if (pit == room->players.end())
        return MoveResult::NOT_FOUND;
    auto &player = pit->second;

    uint8_t bit;
    int dr = 0, dc = 0;
    if (dir == "up")
    {
        bit = maze::NORTH;
        dr = -1;
    }
    else if (dir == "down")
    {
        bit = maze::SOUTH;
        dr = 1;
    }
    else if (dir == "left")
    {
        bit = maze::WEST;
        dc = -1;
    }
    else if (dir == "right")
    {
        bit = maze::EAST;
        dc = 1;
    }
    else
    {
        return MoveResult::INVALID_MOVE;
    }

    int cell = player.row * room->size + player.col;
    if (room->walls[cell] & bit)
        return MoveResult::INVALID_MOVE;

    int nr = player.row + dr;
    int nc = player.col + dc;
    if (nr < 0 || nr >= room->size || nc < 0 || nc >= room->size)
        return MoveResult::INVALID_MOVE;

    player.row = nr;
    player.col = nc;
    newRow = nr;
    newCol = nc;

    int center = room->size / 2;
    if (nr == center && nc == center)
    {
        room->finished = true;
        room->winner = username;
        return MoveResult::WON;
    }
    return MoveResult::MOVED;
}

void GameRoomManager::broadcast(const std::string &code, const Json::Value &msg, const std::string &exceptUsername)
{
    std::vector<drogon::WebSocketConnectionPtr> targets;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = rooms_.find(code);
        if (it == rooms_.end())
            return;
        for (auto &[uname, state] : it->second->players)
        {
            if (uname != exceptUsername && state.conn)
                targets.push_back(state.conn);
        }
    }

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    std::string payload = Json::writeString(builder, msg);
    for (auto &conn : targets)
        conn->send(payload);
}

Json::Value GameRoomManager::playersJson(const std::string &code)
{
    std::lock_guard<std::mutex> lock(mutex_);
    Json::Value arr(Json::arrayValue);
    auto it = rooms_.find(code);
    if (it == rooms_.end())
        return arr;
    for (auto &[uname, state] : it->second->players)
    {
        Json::Value p;
        p["username"] = state.username;
        p["row"] = state.row;
        p["col"] = state.col;
        p["color"] = state.color;
        arr.append(p);
    }
    return arr;
}
