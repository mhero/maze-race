#include <drogon/drogon.h>

using namespace drogon;

namespace
{
void ensureSchema()
{
    auto db = app().getDbClient();
    db->execSqlSync(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "salt TEXT NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "created_at TEXT NOT NULL)");

    db->execSqlSync(
        "CREATE TABLE IF NOT EXISTS games ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "room_code TEXT NOT NULL,"
        "winner TEXT NOT NULL,"
        "created_at TEXT NOT NULL)");

    LOG_INFO << "Database schema ready";
}
}  // namespace

int main()
{
    app().loadConfigFile(std::string(SOURCE_DIR) + "/config.json");

    // Basic permissive CORS so the Vite dev server (localhost:5173) can talk
    // to the API (localhost:5555) during local development / showcasing.
    app().registerPreRoutingAdvice(
        [](const HttpRequestPtr &req, AdviceCallback &&acb, AdviceChainCallback &&accb) {
            if (req->method() == Options)
            {
                auto resp = HttpResponse::newHttpResponse();
                resp->addHeader("Access-Control-Allow-Origin", "*");
                resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
                resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
                acb(resp);
                return;
            }
            accb();
        });

    app().registerPostHandlingAdvice(
        [](const HttpRequestPtr &, const HttpResponsePtr &resp) {
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        });

    app().registerBeginningAdvice([]() { ensureSchema(); });

    LOG_INFO << "Maze Race server starting on port 5555";
    app().run();
    return 0;
}
