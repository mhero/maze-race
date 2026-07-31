#pragma once
#include <cstdint>
#include <random>
#include <vector>

// A simple recursive-backtracker maze generator. Each cell stores a 4-bit
// mask of which walls are present (1=up N, 2=right E, 4=down S, 8=left W).
namespace maze
{
constexpr uint8_t NORTH = 1;
constexpr uint8_t EAST = 2;
constexpr uint8_t SOUTH = 4;
constexpr uint8_t WEST = 8;

inline uint8_t opposite(uint8_t dir)
{
    switch (dir)
    {
        case NORTH:
            return SOUTH;
        case SOUTH:
            return NORTH;
        case EAST:
            return WEST;
        case WEST:
            return EAST;
    }
    return 0;
}

inline std::vector<uint8_t> generate(int size, unsigned seed = std::random_device{}())
{
    std::vector<uint8_t> walls(static_cast<size_t>(size) * size, NORTH | EAST | SOUTH | WEST);
    std::vector<bool> visited(static_cast<size_t>(size) * size, false);
    std::mt19937 rng(seed);

    struct Dir
    {
        uint8_t bit;
        int dr;
        int dc;
    };
    const std::vector<Dir> dirs = {{NORTH, -1, 0}, {EAST, 0, 1}, {SOUTH, 1, 0}, {WEST, 0, -1}};

    std::vector<int> stack;
    int start = 0;
    visited[start] = true;
    stack.push_back(start);

    while (!stack.empty())
    {
        int cur = stack.back();
        int row = cur / size;
        int col = cur % size;

        std::vector<int> candidateDirs;
        for (size_t i = 0; i < dirs.size(); ++i)
        {
            int nr = row + dirs[i].dr;
            int nc = col + dirs[i].dc;
            if (nr < 0 || nr >= size || nc < 0 || nc >= size)
                continue;
            int ncell = nr * size + nc;
            if (!visited[ncell])
                candidateDirs.push_back(static_cast<int>(i));
        }

        if (candidateDirs.empty())
        {
            stack.pop_back();
            continue;
        }

        std::uniform_int_distribution<size_t> dist(0, candidateDirs.size() - 1);
        const Dir &chosen = dirs[candidateDirs[dist(rng)]];
        int nr = row + chosen.dr;
        int nc = col + chosen.dc;
        int ncell = nr * size + nc;

        walls[cur] &= ~chosen.bit;
        walls[ncell] &= ~opposite(chosen.bit);

        visited[ncell] = true;
        stack.push_back(ncell);
    }

    return walls;
}
}  // namespace maze
