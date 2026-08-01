#include "../services/JwtService.h"
#include <gtest/gtest.h>

// Note: JwtService reads its secret/expiry from drogon::app().getCustomConfig(),
// which is empty in this test binary (no config.json is loaded). That's fine —
// sign() and verify() both read the same (empty) secret, so round-tripping is
// still meaningful. What we can't unit test here without either sleeping in a
// test or adding a clock-injection seam is actual token expiry; that's a known
// gap, left as a manual/integration check.

TEST(JwtService, SignThenVerifyRoundTrips)
{
    auto token = JwtService::instance().sign("alice");
    auto result = JwtService::instance().verify(token);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "alice");
}

TEST(JwtService, RejectsGarbageTokens)
{
    EXPECT_FALSE(JwtService::instance().verify("not.a.token").has_value());
    EXPECT_FALSE(JwtService::instance().verify("").has_value());
    EXPECT_FALSE(JwtService::instance().verify("only.two").has_value());
}

TEST(JwtService, RejectsTamperedSignature)
{
    auto token = JwtService::instance().sign("bob");
    std::string tampered = token;
    tampered.back() = (tampered.back() == 'A') ? 'B' : 'A';

    EXPECT_FALSE(JwtService::instance().verify(tampered).has_value());
}

TEST(JwtService, RejectsTamperedPayload)
{
    auto token = JwtService::instance().sign("carol");
    auto firstDot = token.find('.');
    auto secondDot = token.find('.', firstDot + 1);
    ASSERT_NE(secondDot, std::string::npos);

    std::string tampered = token;
    tampered[firstDot + 1] = (tampered[firstDot + 1] == 'A') ? 'B' : 'A';

    EXPECT_FALSE(JwtService::instance().verify(tampered).has_value());
}

TEST(JwtService, DifferentUsersGetDifferentTokens)
{
    auto tokenA = JwtService::instance().sign("dave");
    auto tokenB = JwtService::instance().sign("erin");
    EXPECT_NE(tokenA, tokenB);
}
