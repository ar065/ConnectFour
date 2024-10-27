#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>
#include <utility>
#include <expected>
#include <span>
#include <vector>
#include <ranges>
#include <format>
#include <print>
#include <stdexcept>

struct CellPosition {
    uint16_t row;
    uint16_t col;

    auto operator<=>(const CellPosition&) const = default;
};

struct WinResult {
    bool hasWon{false};
    uint8_t winner{0};
    std::vector<CellPosition> winningCells;

    auto operator<=>(const WinResult&) const = default;
};

struct GameResult {
    bool win{};
    bool draw{};
    std::optional<uint8_t> winner;

    auto operator<=>(const GameResult&) const = default;
};

template<>
struct std::formatter<GameResult> : std::formatter<string_view> {
    static auto format(const GameResult& result, format_context& ctx) {
        return format_to(ctx.out(),
            "GameResult{{win: {}, draw: {}, winner: {}}}",
            result.win,
            result.draw,
            result.winner.has_value() ? std::to_string(*result.winner) : "none"
        );
    }
};

class Board {
public:
    const uint16_t width;
    const uint16_t height;
    std::vector<uint8_t> board;

    // Constructor with size validation
    explicit Board(const uint16_t width, const uint16_t height)
        : width(width)
        , height(height)
        , maxMoves(width * height)
    {
        if (width == 0 || height == 0) {
            throw std::invalid_argument("Board dimensions must be positive");
        }
        if (width > 1000 || height > 1000) {
            throw std::invalid_argument("Board dimensions too large");
        }

        try {
            board.resize(maxMoves, 0);
            heights.resize(width, 0);
        } catch (const std::bad_alloc&) {
            throw std::runtime_error("Failed to allocate board memory");
        }
    }

    // Move operations
    Board(Board&&) noexcept = default;
    Board& operator=(Board&&) noexcept = delete;

    // Delete copy operations
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    [[nodiscard]] std::expected<std::pair<uint16_t, uint16_t>, std::string>
    place(uint16_t col, uint8_t player) noexcept;

    [[nodiscard]] GameResult
    checkWin(std::pair<uint16_t, uint16_t> lastMove) const noexcept;

    [[nodiscard]] WinResult
    checkWinDetailed(uint16_t rowPlayed, uint16_t colPlayed) const noexcept;

    [[nodiscard]] std::span<const uint8_t> view() const noexcept {
        return {board};
    }

    void debug_print() const {
        for (uint16_t i = 0; i < height; ++i) {
            for (uint16_t j = 0; j < width; ++j) {
                std::print("{} ", board[i * width + j]);
            }
            std::println("");
        }
        std::println("Heights: ");
        for (uint16_t i = 0; i < width; ++i) {
            std::print("{} ", heights[i]);
        }
        std::println("");
    }

private:
    std::vector<uint8_t> heights;
    const uint16_t maxMoves;
    uint16_t movesPlayed{0};

    [[nodiscard]] static bool
    isValidPosition(const uint16_t row, const uint16_t col, const uint16_t numRows, const uint16_t numCols) noexcept {
        return row < numRows && col < numCols;
    }
};

#endif // BOARD_HPP