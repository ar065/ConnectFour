#include <ostream>
#include <random>

#include "Board.hpp"
#include "Game.hpp"

class Solver {
private:
    unsigned long long nodeCount;

    int negamax(Game& game) {
        nodeCount++;

        if (game.board.movesPlayed >= game.board.maxMoves) {
            return 0;
        }

        for (int x = 0; x < game.board.width; x++) {
            if (game.board.canPlace(x) && game.place(x)) {
                return (game.board.maxMoves + 1 - game.board.movesPlayed) / 2;
            }
        }

        int bestScore = -game.board.maxMoves;

        for (int x = 0; x < game.board.width; x++) {
            if (game.board.canPlace(x)) {
                // Create a copy of the game here
                Game gameCopy(game);
                gameCopy.place(x);
                const int score = -negamax(gameCopy);
                if (score > bestScore) bestScore = score;
            }
        }

        return bestScore;
    }

    public:
    int solve(const Game& game) {
        nodeCount = 0;
        Game gameCopy(game); // make sure we don't mutate the game lol

        return negamax(gameCopy);
    }

    [[nodiscard]] unsigned long long getNodeCount() const {
        return nodeCount;
    }
};

uint16_t getMove(Game& game) {
    Solver solver{};
    std::vector<uint16_t> scores;

    std::random_device rd;
    std::mt19937 gen(rd());

    for (uint16_t col = 0; col < game.board.width; col++) {
        Game gameCopy(game);
        gameCopy.place(col);
        int score = solver.solve(gameCopy);

        std::println("Score: {}", score);
        scores.push_back(score);
    }

    auto maxIt = std::max_element(scores.begin(), scores.end());
    uint16_t maxIndex = std::distance(scores.begin(), maxIt);
    int maxScore = *maxIt;

    // std::cout << "Highest Score: " << maxScore << " at index: " << maxIndex << std::endl;
    std::println("Highest Score: {} at index: {}", maxScore, maxIndex);

    return maxIndex;
}