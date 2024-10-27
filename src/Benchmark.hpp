#pragma once

void runGameBatch(const int numGames, const std::vector<int>& moves, std::atomic<int>& errorCount);
void runBenchmark();