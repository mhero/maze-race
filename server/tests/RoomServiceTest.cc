#include "../services/RoomService.h"
#include <gtest/gtest.h>

TEST(RoomService, CreateRoomDefaultsToFifteenForInvalidSizes)
{
    EXPECT_EQ(RoomService::createRoom(0)["size"].asInt(), 15);
    EXPECT_EQ(RoomService::createRoom(8)["size"].asInt(), 15);    // even, rejected
    EXPECT_EQ(RoomService::createRoom(999)["size"].asInt(), 15);  // out of range
}

TEST(RoomService, CreateRoomAcceptsValidOddSizeInRange)
{
    EXPECT_EQ(RoomService::createRoom(11)["size"].asInt(), 11);
}

TEST(RoomService, GetRoomStatusReflectsCreatedRoom)
{
    auto created = RoomService::createRoom(9);
    auto status = RoomService::getRoomStatus(created["code"].asString());

    ASSERT_TRUE(status.has_value());
    EXPECT_EQ((*status)["size"].asInt(), 9);
    EXPECT_EQ((*status)["playerCount"].asInt(), 0);
    EXPECT_FALSE((*status)["finished"].asBool());
}

TEST(RoomService, GetRoomStatusReturnsNulloptForUnknownCode)
{
    EXPECT_FALSE(RoomService::getRoomStatus("ZZZZZZ").has_value());
}
