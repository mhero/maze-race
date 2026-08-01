#include "RoomController.h"
#include "../services/RoomService.h"
#include <drogon/drogon.h>

using namespace drogon;

void RoomController::createRoom(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    int requestedSize = (json && json->isMember("size")) ? (*json)["size"].asInt() : 0;

    callback(HttpResponse::newHttpJsonResponse(RoomService::createRoom(requestedSize)));
}

void RoomController::getRoom(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                              std::string code)
{
    auto status = RoomService::getRoomStatus(code);
    if (!status)
    {
        Json::Value err;
        err["error"] = "room not found";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k404NotFound);
        callback(resp);
        return;
    }

    callback(HttpResponse::newHttpJsonResponse(*status));
}
