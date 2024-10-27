#include <print>
#include <chrono>
#include <thread>
#include <vector>
#include <future>
#include <random>
#include "Game.hpp"
#include "BoardPrinter.hpp"

// Function to run a batch of games
void runGameBatch(int numGames, const std::vector<int>& moves, std::atomic<int>& errorCount) {
    for (int i = 0; i < numGames; ++i) {
        try {
            Game game(7, 6, 2); // Create a new game for each iteration
            for (const int move : moves) {
                auto _ = game.place(move);
            }
        } catch (const std::exception& e) {
            ++errorCount;
        }
    }
}

int main() {
    constexpr bool run_benchmark = false;

    if (run_benchmark) {
        std::vector<int> moves = { 0, 1, 0, 1, 0, 1, 0 };
        constexpr int numberOfGames = 1'000'000'000;

        // Get the number of available CPU cores
        const unsigned int numCores = std::thread::hardware_concurrency();
        std::println("Using {} CPU cores", numCores);

        // Calculate games per thread
        const int gamesPerThread = numberOfGames / numCores;
        const int remainingGames = numberOfGames % numCores;

        std::vector<std::thread> threads;
        std::atomic<int> errorCount(0);

        const auto start = std::chrono::high_resolution_clock::now();

        // Launch threads
        for (unsigned int i = 0; i < numCores; ++i) {
            int threadGames = gamesPerThread + (i == numCores - 1 ? remainingGames : 0);
            threads.emplace_back(runGameBatch, threadGames, std::ref(moves), std::ref(errorCount));
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> duration = end - start;

        std::println("{} games took {} seconds", numberOfGames, duration.count());
        if (errorCount > 0) {
            std::println("Encountered {} errors during simulation", errorCount.load());
        }
    } else {
        Game game(7, 6, 2);

        std::optional<MoveResult> gameResult = std::nullopt;

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 6);

        while (!gameResult) {
            gameResult = game.place(dist6(rng));
            BoardPrinter::printBoard(game.board);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Print final result
        if (gameResult) {
            const auto& [move, result] = *gameResult;
            if (result.win) {
                const auto winDetails = game.board.checkWinDetailed(move.first, move.second);

                BoardPrinter::printBoard(game.board, &winDetails.winningCells);
                std::println("Player {} won!", game.getCurrentPlayer());
                // std::println("Winning move: {} {}", move.first, move.second);
            } else if (result.draw) {
                std::println("Game ended in a draw!");
            }
        }
    }

    return 0;
}