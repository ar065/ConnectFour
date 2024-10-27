#pragma once
#include <cstdint>

#include "Board.hpp"
#include "Game.hpp"

class Solver {
private:
    unsigned long long nodeCount;

    int negamax(Game& game);

public:
    int solve(const Game& game);
    [[nodiscard]] unsigned long long getNodeCount() const;
};

uint16_t getMove(Game& game);