#include "GameController.h"
#include "../services/GameService.h"
#include <drogon/drogon.h>

using namespace drogon;

void GameController::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &conn)
{
    auto token = req->getParameter("token");
    auto roomCode = req->getParameter("room");

    auto result = GameService::handleConnect(token, roomCode, conn);
    if (!result.ok)
    {
        conn->forceClose();
        return;
    }

    conn->setContext(std::make_shared<WsPlayerContext>(WsPlayerContext{result.username, roomCode}));
}

void GameController::handleNewMessage(const WebSocketConnectionPtr &conn, std::string &&message,
                                       const WebSocketMessageType &type)
{
    if (type != WebSocketMessageType::Text)
        return;

    auto ctx = conn->getContext<WsPlayerContext>();
    if (!ctx)
        return;

    GameService::handleMessage(*ctx, message);
}

void GameController::handleConnectionClosed(const WebSocketConnectionPtr &conn)
{
    auto ctx = conn->getContext<WsPlayerContext>();
    if (!ctx)
        return;

    GameService::handleDisconnect(*ctx);
}
