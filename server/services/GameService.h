#pragma once
#include <drogon/WebSocketConnection.h>
#include <string>
#include "GameRoomManager.h"

// Business logic for the live game session. GameController (the
// WebSocketController) only knows how to read a Drogon connection/message —
// everything about what a connect/move/disconnect actually *means* lives
// here: JWT verification, joining a room, validating moves, building the
// outgoing JSON messages, and recording wins.
class GameService
{
  public:
    struct JoinResult
    {
        bool ok = false;
        std::string username;  // only meaningful when ok == true
    };

    // Verifies the token, validates/joins the room, and sends the "joined"
    // payload to `conn` plus a "player_joined" broadcast to the rest of the
    // room. On failure, sends an "error" message on `conn` itself.
    static JoinResult handleConnect(const std::string &token, const std::string &roomCode,
                                     const drogon::WebSocketConnectionPtr &conn);

    // Parses an incoming WS text message and, if it's a valid move, applies
    // it and broadcasts the resulting position / game_over as needed.
    static void handleMessage(const WsPlayerContext &ctx, const std::string &rawMessage);

    static void handleDisconnect(const WsPlayerContext &ctx);
};
