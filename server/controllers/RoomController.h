#pragma once
#include <drogon/HttpController.h>

class RoomController : public drogon::HttpController<RoomController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RoomController::createRoom, "/api/rooms", drogon::Post, "JwtAuthFilter");
    ADD_METHOD_TO(RoomController::getRoom, "/api/rooms/{code}", drogon::Get);
    METHOD_LIST_END

    void createRoom(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void getRoom(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                 std::string code);
};
