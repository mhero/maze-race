#include "../services/GameRoomManager.h"
#include "../services/MazeGenerator.h"
#include <gtest/gtest.h>

TEST(GameRoomManager, CreateRoomGeneratesMazeOfRequestedSize)
{
    auto room = GameRoomManager::instance().createRoom(9);
    EXPECT_EQ(room->size, 9);
    EXPECT_EQ(room->walls.size(), 81u);
    EXPECT_FALSE(room->code.empty());
    EXPECT_FALSE(room->finished);
}

TEST(GameRoomManager, CreateRoomAssignsUniqueCodes)
{
    auto a = GameRoomManager::instance().createRoom(9);
    auto b = GameRoomManager::instance().createRoom(9);
    EXPECT_NE(a->code, b->code);
}

TEST(GameRoomManager, GetRoomReturnsNullForUnknownCode)
{
    EXPECT_EQ(GameRoomManager::instance().getRoom("NOPE99"), nullptr);
}

TEST(GameRoomManager, JoinRoomAssignsCornersAndRejectsAfterFour)
{
    auto room = GameRoomManager::instance().createRoom(9);
    int row, col;
    std::string color;

    ASSERT_TRUE(GameRoomManager::instance().joinRoom(room->code, "p1", nullptr, row, col, color));
    EXPECT_EQ(row, 0);
    EXPECT_EQ(col, 0);

    ASSERT_TRUE(GameRoomManager::instance().joinRoom(room->code, "p2", nullptr, row, col, color));
    EXPECT_EQ(row, 0);
    EXPECT_EQ(col, 8);

    ASSERT_TRUE(GameRoomManager::instance().joinRoom(room->code, "p3", nullptr, row, col, color));
    EXPECT_EQ(row, 8);
    EXPECT_EQ(col, 0);

    ASSERT_TRUE(GameRoomManager::instance().joinRoom(room->code, "p4", nullptr, row, col, color));
    EXPECT_EQ(row, 8);
    EXPECT_EQ(col, 8);

    EXPECT_FALSE(GameRoomManager::instance().joinRoom(room->code, "p5", nullptr, row, col, color))
        << "a fifth player should not be able to join a 4-corner room";
}

TEST(GameRoomManager, JoinRoomRejectsDuplicateUsername)
{
    auto room = GameRoomManager::instance().createRoom(9);
    int row, col;
    std::string color;

    ASSERT_TRUE(GameRoomManager::instance().joinRoom(room->code, "dupe", nullptr, row, col, color));
    EXPECT_FALSE(GameRoomManager::instance().joinRoom(room->code, "dupe", nullptr, row, col, color));
}

TEST(GameRoomManager, TryMoveRespectsWalls)
{
    auto room = GameRoomManager::instance().createRoom(3);

    // Overwrite with a known 3x3 layout so movement is deterministic: cell 0
    // is open only to the east (cell 1), everything else fully walled.
    room->walls.assign(9, maze::NORTH | maze::EAST | maze::SOUTH | maze::WEST);
    room->walls[0] &= ~maze::EAST;
    room->walls[1] &= ~maze::WEST;

    int row, col;
    std::string color;
    GameRoomManager::instance().joinRoom(room->code, "mover", nullptr, row, col, color);

    int newRow, newCol;
    EXPECT_EQ(GameRoomManager::instance().tryMove(room->code, "mover", "down", newRow, newCol),
              MoveResult::INVALID_MOVE);
    EXPECT_EQ(GameRoomManager::instance().tryMove(room->code, "mover", "up", newRow, newCol),
              MoveResult::INVALID_MOVE);

    EXPECT_EQ(GameRoomManager::instance().tryMove(room->code, "mover", "right", newRow, newCol), MoveResult::MOVED);
    EXPECT_EQ(newRow, 0);
    EXPECT_EQ(newCol, 1);
}

TEST(GameRoomManager, ReachingCenterWinsAndLocksTheRoom)
{
    auto room = GameRoomManager::instance().createRoom(3);
    room->walls.assign(9, 0);  // fully open, so the path to center is unambiguous

    int row, col;
    std::string color;
    GameRoomManager::instance().joinRoom(room->code, "winner", nullptr, row, col, color);

    int newRow, newCol;
    GameRoomManager::instance().tryMove(room->code, "winner", "right", newRow, newCol);  // (0,0) -> (0,1)
    auto result = GameRoomManager::instance().tryMove(room->code, "winner", "down", newRow, newCol);  // -> (1,1)

    EXPECT_EQ(result, MoveResult::WON);
    EXPECT_EQ(newRow, 1);
    EXPECT_EQ(newCol, 1);

    EXPECT_EQ(GameRoomManager::instance().tryMove(room->code, "winner", "up", newRow, newCol),
              MoveResult::ROOM_FINISHED);
}

TEST(GameRoomManager, LeaveRoomRemovesPlayer)
{
    auto room = GameRoomManager::instance().createRoom(9);
    int row, col;
    std::string color;
    GameRoomManager::instance().joinRoom(room->code, "leaver", nullptr, row, col, color);

    EXPECT_EQ(GameRoomManager::instance().playersJson(room->code).size(), 1u);

    GameRoomManager::instance().leaveRoom(room->code, "leaver");

    EXPECT_EQ(GameRoomManager::instance().playersJson(room->code).size(), 0u);
}
