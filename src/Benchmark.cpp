#include <atomic>
#include <exception>
#include <thread>

#include "Game.hpp"

void runGameBatch(const int numGames, const std::vector<int>& moves, std::atomic<int>& errorCount) {
    for (int i = 0; i < numGames; ++i) {
        try {
            Game game(7, 6, 2);
            for (const int move : moves) {
                auto _ = game.place(move);
            }
        } catch (const std::exception& e) {
            ++errorCount;
        }
    }
}

void runBenchmark() {
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
}