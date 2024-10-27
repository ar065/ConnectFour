#include <random>

#include "Board.hpp"

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 6);

uint16_t getMove(const Board& boardInstance) {
    const uint16_t col = dist6(rng);

    return col;
}
