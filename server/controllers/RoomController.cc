#include "RoomController.h"
#include "../services/GameRoomManager.h"
#include <drogon/drogon.h>

using namespace drogon;

void RoomController::createRoom(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    int size = 15;  // odd sizes only, so there's a well-defined single center cell
    auto json = req->getJsonObject();
    if (json && json->isMember("size"))
    {
        int requested = (*json)["size"].asInt();
        if (requested >= 9 && requested <= 25 && requested % 2 == 1)
            size = requested;
    }

    auto room = GameRoomManager::instance().createRoom(size);

    Json::Value res;
    res["code"] = room->code;
    res["size"] = room->size;
    callback(HttpResponse::newHttpJsonResponse(res));
}

void RoomController::getRoom(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                              std::string code)
{
    auto room = GameRoomManager::instance().getRoom(code);
    if (!room)
    {
        Json::Value err;
        err["error"] = "room not found";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k404NotFound);
        callback(resp);
        return;
    }

    Json::Value res;
    res["code"] = room->code;
    res["size"] = room->size;
    res["playerCount"] = static_cast<int>(room->players.size());
    res["finished"] = room->finished;
    callback(HttpResponse::newHttpJsonResponse(res));
}
