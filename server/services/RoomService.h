#pragma once
#include <json/json.h>
#include <optional>
#include <string>

// Business logic for REST room endpoints — size validation and JSON
// shaping. RoomController just calls this and turns the result into an
// HttpResponse.
class RoomService
{
  public:
    static Json::Value createRoom(int requestedSize);
    static std::optional<Json::Value> getRoomStatus(const std::string &code);
};
