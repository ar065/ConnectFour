#include <print>
#include <chrono>
#include "Game.hpp"

int main() {
    constexpr bool run_benchmark = true;

    if (run_benchmark)
    {
        std::vector<int> moves = { 0, 1, 0, 1, 0, 1, 0 };

        constexpr int numberOfGames = 1'000'000'000;

        const auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < numberOfGames; ++i) {
            try {
                Game game(7, 6, 2); // Create a new game for each iteration

                for (const int move : moves) {
                    auto _ = game.place(move);
                }

            } catch (const std::exception& e) {
                std::println("Error during game simulation: {}", e.what());
            }
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> duration = end - start;

        std::println("{} games took {} seconds", numberOfGames, duration.count());
    } else
    {

    }

    return 0;
}
