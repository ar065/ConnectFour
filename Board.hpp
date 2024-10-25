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
    const std::size_t width;
    const std::size_t height;

    // Constructor with size validation
    explicit Board(std::size_t width, std::size_t height)
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

    [[nodiscard]] std::expected<std::pair<std::size_t, std::size_t>, std::string>
    place(std::size_t col, uint8_t player) noexcept;

    [[nodiscard]] GameResult
    checkWin(std::pair<std::size_t, std::size_t> lastMove) const noexcept;

    [[nodiscard]] std::span<const uint8_t> view() const noexcept {
        return {board};
    }

    void debug_print() const {
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                std::print("{} ", board[i * width + j]);
            }
            std::println("");
        }
        std::println("Heights: ");
        for (size_t i = 0; i < width; ++i) {
            std::print("{} ", heights[i]);
        }
        std::println("");
    }

private:
    std::vector<uint8_t> board;
    std::vector<uint8_t> heights;
    const std::size_t maxMoves;
    std::size_t movesPlayed{0};
};

#endif // BOARD_HPP