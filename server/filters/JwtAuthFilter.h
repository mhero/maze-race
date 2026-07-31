#pragma once
#include <drogon/HttpFilter.h>

// Protects REST endpoints that require a logged-in user. On success, stashes
// the authenticated username on the request's attributes under "username".
class JwtAuthFilter : public drogon::HttpFilter<JwtAuthFilter>
{
  public:
    void doFilter(const drogon::HttpRequestPtr &req, drogon::FilterCallback &&fcb,
                  drogon::FilterChainCallback &&fccb) override;
};
