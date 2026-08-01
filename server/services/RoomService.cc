#include "RoomService.h"
#include "GameRoomManager.h"

namespace
{
constexpr int kDefaultSize = 15;  // odd, so there's a well-defined single center cell
constexpr int kMinSize = 9;
constexpr int kMaxSize = 25;

int clampSize(int requested)
{
    if (requested >= kMinSize && requested <= kMaxSize && requested % 2 == 1)
        return requested;
    return kDefaultSize;
}
}  // namespace

Json::Value RoomService::createRoom(int requestedSize)
{
    auto room = GameRoomManager::instance().createRoom(clampSize(requestedSize));

    Json::Value res;
    res["code"] = room->code;
    res["size"] = room->size;
    return res;
}

std::optional<Json::Value> RoomService::getRoomStatus(const std::string &code)
{
    auto room = GameRoomManager::instance().getRoom(code);
    if (!room)
        return std::nullopt;

    Json::Value res;
    res["code"] = room->code;
    res["size"] = room->size;
    res["playerCount"] = static_cast<int>(room->players.size());
    res["finished"] = room->finished;
    return res;
}
