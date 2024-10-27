#ifndef GAME_HPP
#define GAME_HPP

#include "Board.hpp"
#include <cstdint>
#include <optional>

struct MoveResult {
    std::pair<uint16_t, uint16_t> move;
    GameResult result;
};

class Game {
public:
    Board board;
    const uint8_t numberOfPlayers;
    const uint16_t width;
    const uint16_t height;

    // Constructor with parameter validation
    explicit Game(const uint16_t width, const uint16_t height, const uint8_t numberOfPlayers)
        : board(width, height)
        , numberOfPlayers(numberOfPlayers)
        , width(width)
        , height(height)
        , currentPlayer(1)
    {
        if (numberOfPlayers < 2 || numberOfPlayers > 6) {
            throw std::invalid_argument("Number of players must be between 2 and 6");
        }
    }

    // Move operations
    Game(Game&&) noexcept = default;
    Game& operator=(Game&&) noexcept = delete;

    // Delete copy operations
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    [[nodiscard]] std::optional<MoveResult> place(const uint16_t col) {
        const auto moveResult = board.place(col, currentPlayer);
        if (!moveResult) {
            return std::nullopt;
        }

        if (const auto gameResult = board.checkWin(*moveResult); gameResult.win || gameResult.draw) {
            return MoveResult{*moveResult, gameResult};
        }

        currentPlayer = (currentPlayer % numberOfPlayers) + 1;
        return std::nullopt;
    }

    [[nodiscard]] uint8_t getCurrentPlayer() const noexcept {
        return currentPlayer;
    }

private:
    uint8_t currentPlayer;
};

#endif // GAME_HPP