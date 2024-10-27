#include "Board.hpp"
#include <algorithm>
#include <ranges>

namespace {
    constexpr GameResult DRAW_RESULT = {false, true, std::nullopt};
    constexpr GameResult NO_WIN_RESULT = {false, false, std::nullopt};

    constexpr GameResult WIN_RESULT(uint8_t player) {
        return {true, false, player};
    }
}

std::expected<std::pair<std::size_t, std::size_t>, std::string>
Board::place(std::size_t col, const uint8_t player) noexcept {
    if (col >= width) {
        return std::unexpected("Column out of bounds");
    }

    const auto height = heights[col];
    if (height >= this->height) {
        return std::unexpected("Column is full");
    }

    const auto row = this->height - 1 - height;
    board[row * width + col] = player;
    heights[col] = height + 1;
    movesPlayed++;
    return std::pair(row, col);
}

GameResult Board::checkWin(std::pair<std::size_t, std::size_t> lastMove) const noexcept {
    // Early exit if win is impossible
    if (movesPlayed < 7) return NO_WIN_RESULT;

    const auto [row, col] = lastMove;
    const auto pos = row * width + col;
    const auto player = board[pos];

    if (player == 0) return NO_WIN_RESULT;

    // Horizontal check using ranges
    const auto rowStart = row * width;
    const auto rowView = std::span(board).subspan(rowStart, width);
    
    auto window = std::views::counted(rowView.begin() + std::max<int>(0, col - 3), 7);
    auto consecutive = std::views::adjacent<4>(window);
    
    auto hasWin = std::ranges::any_of(consecutive, [player](const auto& group) {
        const auto& [a, b, c, d] = group;
        return a == player && b == player && c == player && d == player;
    });

    if (hasWin) return WIN_RESULT(player);

    // Vertical check - only need to check down
    if (row <= height - 4) {
        if (auto vPos = pos + width; board[vPos] == player) {
            vPos += width;
            if (board[vPos] == player) {
                vPos += width;
                if (board[vPos] == player) {
                    return WIN_RESULT(player);
                }
            }
        }
    }

    // Only do diagonal checks if we have enough space
    if (row <= height - 4) {
        // Diagonal check (↘)
        if (col <= width - 4) {
            if (auto dPos = pos + width + 1; board[dPos] == player) {
                dPos += width + 1;
                if (board[dPos] == player) {
                    dPos += width + 1;
                    if (board[dPos] == player) {
                        return WIN_RESULT(player);
                    }
                }
            }
        }

        // Diagonal check (↙)
        if (col >= 3) {
            if (auto dPos = pos + width - 1; board[dPos] == player) {
                dPos += width - 1;
                if (board[dPos] == player) {
                    dPos += width - 1;
                    if (board[dPos] == player) {
                        return WIN_RESULT(player);
                    }
                }
            }
        }
    }

    return movesPlayed == maxMoves ? DRAW_RESULT : NO_WIN_RESULT;
}