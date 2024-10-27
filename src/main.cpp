#include <print>
#include <chrono>
#include <thread>
#include <future>
#include <iostream>
#include <random>

#include "AI.hpp"
#include "Game.hpp"
#include "Benchmark.hpp"
#include "BoardPrinter.hpp"

uint16_t getColFromInput() {
    std::print("Col: ");
    std::string input;
    std::getline(std::cin, input);

    try {
        const int col = std::stoi(input);
        if (col < 0) {
            throw std::out_of_range("Negative number");
        }
        return static_cast<uint16_t>(col);
    }
    catch (const std::exception&) {
        throw std::out_of_range("Invalid input");
    }
}

int main() {
    constexpr bool run_benchmark = false;

    if (run_benchmark) {
        runBenchmark();
    } else {
        Game game(7, 6, 2);

        std::optional<MoveResult> gameResult = std::nullopt;

        while (!gameResult) {
            if (game.getCurrentPlayer() == 1) {
                const auto col = getColFromInput();
                gameResult = game.place(col);
            } else {
                const auto col = getMove(game.board);
                std::println("AI Play: Col: {}", col);
                gameResult = game.place(col);
            }
            if (!gameResult) {
                BoardPrinter::printBoard(game.board);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        // Print final result
        if (gameResult) {
            if (const auto& [move, result] = *gameResult; result.win) {
                const auto [hasWon, winner, winningCells] = game.board.checkWinDetailed(move.first, move.second);

                BoardPrinter::printBoard(game.board, &winningCells);
                std::println("Player {} won!", game.getCurrentPlayer());
                // std::println("Winning move: {} {}", move.first, move.second);
            } else if (result.draw) {
                std::println("Game ended in a draw!");
            }
        }
    }

    return 0;
}