#include "../services/PasswordService.h"
#include <gtest/gtest.h>

TEST(PasswordService, VerifyAcceptsCorrectPassword)
{
    std::string salt, hash;
    PasswordService::hashPassword("correct horse battery staple", salt, hash);
    EXPECT_TRUE(PasswordService::verifyPassword("correct horse battery staple", salt, hash));
}

TEST(PasswordService, VerifyRejectsWrongPassword)
{
    std::string salt, hash;
    PasswordService::hashPassword("correct horse battery staple", salt, hash);
    EXPECT_FALSE(PasswordService::verifyPassword("wrong password", salt, hash));
}

TEST(PasswordService, VerifyIsDeterministicForTheSameSaltAndHash)
{
    std::string salt, hash;
    PasswordService::hashPassword("hunter2", salt, hash);

    EXPECT_TRUE(PasswordService::verifyPassword("hunter2", salt, hash));
    EXPECT_TRUE(PasswordService::verifyPassword("hunter2", salt, hash));
}

TEST(PasswordService, DifferentCallsProduceDifferentSaltsAndHashes)
{
    std::string salt1, hash1, salt2, hash2;
    PasswordService::hashPassword("same password", salt1, hash1);
    PasswordService::hashPassword("same password", salt2, hash2);

    EXPECT_NE(salt1, salt2) << "salts should be randomly generated per call";
    EXPECT_NE(hash1, hash2) << "hashes should differ because salts differ";
}
