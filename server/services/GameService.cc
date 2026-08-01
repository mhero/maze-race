#include "GameService.h"
#include "JwtService.h"
#include "LeaderboardService.h"
#include "Utils.h"
#include <sstream>

using namespace drogon;

namespace
{
void sendError(const WebSocketConnectionPtr &conn, const std::string &message)
{
    Json::Value err;
    err["type"] = "error";
    err["message"] = message;
    conn->send(jsonToString(err));
}
}  // namespace

GameService::JoinResult GameService::handleConnect(const std::string &token, const std::string &roomCode,
                                                     const WebSocketConnectionPtr &conn)
{
    auto usernameOpt = JwtService::instance().verify(token);
    if (!usernameOpt || roomCode.empty())
    {
        sendError(conn, "invalid token or missing room code");
        return {};
    }
    std::string username = *usernameOpt;

    auto room = GameRoomManager::instance().getRoom(roomCode);
    if (!room)
    {
        sendError(conn, "room not found");
        return {};
    }

    int row, col;
    std::string color;
    if (!GameRoomManager::instance().joinRoom(roomCode, username, conn, row, col, color))
    {
        sendError(conn, "room is full, already finished, or you're already connected");
        return {};
    }

    Json::Value joined;
    joined["type"] = "joined";
    joined["you"]["username"] = username;
    joined["you"]["row"] = row;
    joined["you"]["col"] = col;
    joined["you"]["color"] = color;
    joined["maze"]["size"] = room->size;
    Json::Value wallsArr(Json::arrayValue);
    for (auto w : room->walls)
        wallsArr.append(w);
    joined["maze"]["walls"] = wallsArr;
    joined["players"] = GameRoomManager::instance().playersJson(roomCode);
    conn->send(jsonToString(joined));

    Json::Value playerJoined;
    playerJoined["type"] = "player_joined";
    playerJoined["username"] = username;
    playerJoined["row"] = row;
    playerJoined["col"] = col;
    playerJoined["color"] = color;
    GameRoomManager::instance().broadcast(roomCode, playerJoined, username);

    return {true, username};
}

void GameService::handleMessage(const WsPlayerContext &ctx, const std::string &rawMessage)
{
    Json::Value msg;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss(rawMessage);
    if (!Json::parseFromStream(builder, iss, &msg, &errs))
        return;

    if (msg.get("type", "").asString() != "move")
        return;

    std::string dir = msg.get("dir", "").asString();
    int newRow = 0, newCol = 0;
    auto result = GameRoomManager::instance().tryMove(ctx.room, ctx.username, dir, newRow, newCol);

    if (result == MoveResult::MOVED || result == MoveResult::WON)
    {
        Json::Value pos;
        pos["type"] = "position";
        pos["username"] = ctx.username;
        pos["row"] = newRow;
        pos["col"] = newCol;
        GameRoomManager::instance().broadcast(ctx.room, pos);
    }

    if (result == MoveResult::WON)
    {
        Json::Value over;
        over["type"] = "game_over";
        over["winner"] = ctx.username;
        GameRoomManager::instance().broadcast(ctx.room, over);
        LeaderboardService::recordWin(ctx.room, ctx.username);
    }
}

void GameService::handleDisconnect(const WsPlayerContext &ctx)
{
    GameRoomManager::instance().leaveRoom(ctx.room, ctx.username);

    Json::Value left;
    left["type"] = "player_left";
    left["username"] = ctx.username;
    GameRoomManager::instance().broadcast(ctx.room, left);
}
