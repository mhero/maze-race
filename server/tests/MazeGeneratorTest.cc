#include "../services/MazeGenerator.h"
#include <gtest/gtest.h>
#include <queue>
#include <vector>

namespace
{
bool canPass(const std::vector<uint8_t> &walls, int from, int to, int size)
{
    int fr = from / size, fc = from % size;
    int tr = to / size, tc = to % size;
    if (tr == fr - 1 && tc == fc)
        return !(walls[from] & maze::NORTH);
    if (tr == fr + 1 && tc == fc)
        return !(walls[from] & maze::SOUTH);
    if (tc == fc - 1 && tr == fr)
        return !(walls[from] & maze::WEST);
    if (tc == fc + 1 && tr == fr)
        return !(walls[from] & maze::EAST);
    return false;
}
}  // namespace

TEST(MazeGenerator, ProducesCorrectCellCount)
{
    auto walls = maze::generate(9, 42);
    EXPECT_EQ(walls.size(), 81u);
}

TEST(MazeGenerator, EveryCellIsReachableFromStart)
{
    const int size = 11;
    auto walls = maze::generate(size, 7);

    std::vector<bool> visited(size * size, false);
    std::queue<int> q;
    q.push(0);
    visited[0] = true;
    int count = 1;

    while (!q.empty())
    {
        int cur = q.front();
        q.pop();
        int row = cur / size, col = cur % size;
        int neighbors[4] = {-1, -1, -1, -1};
        if (row > 0)
            neighbors[0] = cur - size;
        if (row < size - 1)
            neighbors[1] = cur + size;
        if (col > 0)
            neighbors[2] = cur - 1;
        if (col < size - 1)
            neighbors[3] = cur + 1;

        for (int n : neighbors)
        {
            if (n < 0 || visited[n])
                continue;
            if (canPass(walls, cur, n, size))
            {
                visited[n] = true;
                count++;
                q.push(n);
            }
        }
    }

    EXPECT_EQ(count, size * size) << "maze must be fully connected (a perfect maze)";
}

TEST(MazeGenerator, BorderWallsArePreserved)
{
    const int size = 7;
    auto walls = maze::generate(size, 99);

    for (int c = 0; c < size; ++c)
        EXPECT_TRUE(walls[c] & maze::NORTH) << "top row must keep its north wall";
    for (int c = 0; c < size; ++c)
        EXPECT_TRUE(walls[(size - 1) * size + c] & maze::SOUTH) << "bottom row must keep its south wall";
    for (int r = 0; r < size; ++r)
        EXPECT_TRUE(walls[r * size] & maze::WEST) << "left column must keep its west wall";
    for (int r = 0; r < size; ++r)
        EXPECT_TRUE(walls[r * size + size - 1] & maze::EAST) << "right column must keep its east wall";
}

TEST(MazeGenerator, IsDeterministicForAGivenSeed)
{
    auto a = maze::generate(9, 123);
    auto b = maze::generate(9, 123);
    EXPECT_EQ(a, b);
}

TEST(MazeGenerator, OppositeMapsCorrectly)
{
    EXPECT_EQ(maze::opposite(maze::NORTH), maze::SOUTH);
    EXPECT_EQ(maze::opposite(maze::SOUTH), maze::NORTH);
    EXPECT_EQ(maze::opposite(maze::EAST), maze::WEST);
    EXPECT_EQ(maze::opposite(maze::WEST), maze::EAST);
}
