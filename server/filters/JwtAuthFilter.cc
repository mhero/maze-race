#include "JwtAuthFilter.h"
#include "../services/JwtService.h"
#include <drogon/drogon.h>

using namespace drogon;

void JwtAuthFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb, FilterChainCallback &&fccb)
{
    auto authHeader = req->getHeader("Authorization");
    static const std::string prefix = "Bearer ";

    if (authHeader.size() <= prefix.size() || authHeader.compare(0, prefix.size(), prefix) != 0)
    {
        Json::Value err;
        err["error"] = "missing or invalid Authorization header";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k401Unauthorized);
        fcb(resp);
        return;
    }

    std::string token = authHeader.substr(prefix.size());
    auto usernameOpt = JwtService::instance().verify(token);
    if (!usernameOpt)
    {
        Json::Value err;
        err["error"] = "invalid or expired token";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k401Unauthorized);
        fcb(resp);
        return;
    }

    req->attributes()->insert("username", *usernameOpt);
    fccb();
}
