#include <ostream>
#include <random>
#include <limits>
#include <unordered_map>
#include "Board.hpp"
#include "Game.hpp"

// Compact board representation for hash table
struct BoardState {
    std::vector<uint8_t> board;
    uint8_t currentPlayer;

    bool operator==(const BoardState& other) const {
        return board == other.board && currentPlayer == other.currentPlayer;
    }
};

// Custom hash function for BoardState
struct BoardStateHash {
    std::size_t operator()(const BoardState& state) const {
        std::size_t hash = 0;
        for (const auto& cell : state.board) {
            hash = hash * 31 + cell;
        }
        hash = hash * 31 + state.currentPlayer;
        return hash;
    }
};

class Solver {
private:
    unsigned long long nodeCount;
    static constexpr int MAX_DEPTH = 8;
    static constexpr int WIN_SCORE = 1000000;

    // Transposition table entry
    struct TTEntry {
        int score;
        int depth;
        enum class Type { EXACT, LOWER_BOUND, UPPER_BOUND } type;
    };

    std::unordered_map<BoardState, TTEntry, BoardStateHash> transpositionTable;

    // Column ordering for better alpha-beta pruning
    static constexpr std::array<uint16_t, 7> columnOrder = {3, 2, 4, 1, 5, 0, 6};  // Center-first ordering

    // Fast evaluation using bitboards for threat detection
    int evaluatePosition(const Game& game) const {
        const auto& board = game.board;
        int score = 0;

        // Quick center control evaluation
        const uint16_t centerCol = board.width / 2;
        const auto& heights = board.heights;

        // Value center columns more
        score += heights[centerCol] * 3;  // Center column
        if (centerCol > 0) {
            score += heights[centerCol-1] * 2;  // Adjacent to center
            score += heights[centerCol+1] * 2;
        }

        // Check for immediate threats in each column
        for (uint16_t col = 0; col < board.width; col++) {
            const auto height = heights[col];
            if (height >= board.height) continue;

            // Check if placing here would win
            const uint16_t row = board.height - 1 - height;
            const uint16_t pos = row * board.width + col;

            // Horizontal check (most common win condition)
            if (col <= board.width - 4) {
                int count = 0;
                uint8_t lastPlayer = 0;
                for (uint16_t i = 0; i < 4; i++) {
                    const auto piece = board.board[pos + i];
                    if (piece != 0) {
                        if (lastPlayer == 0) {
                            lastPlayer = piece;
                            count = 1;
                        } else if (piece == lastPlayer) {
                            count++;
                        } else {
                            count = 0;
                            break;
                        }
                    }
                }
                if (count == 3) {
                    score += (lastPlayer == game.currentPlayer ? 100 : -100);
                }
            }
        }

        return score;
    }

    int negamax(const Game& game, int depth, int alpha, int beta) {
        nodeCount++;

        // Transposition table lookup
        BoardState state{game.board.board, game.currentPlayer};
        if (auto it = transpositionTable.find(state); it != transpositionTable.end()) {
            const auto& entry = it->second;
            if (entry.depth >= depth) {
                switch (entry.type) {
                    case TTEntry::Type::EXACT:
                        return entry.score;
                    case TTEntry::Type::LOWER_BOUND:
                        alpha = std::max(alpha, entry.score);
                        break;
                    case TTEntry::Type::UPPER_BOUND:
                        beta = std::min(beta, entry.score);
                        break;
                }
                if (alpha >= beta) return entry.score;
            }
        }

        // Check for immediate win using pre-ordered columns
        for (uint16_t col : columnOrder) {
            if (col >= game.board.width) continue;  // Skip if column index exceeds board width
            if (game.board.canPlace(col)) {
                Game testGame(game);
                if (testGame.place(col)) {
                    return WIN_SCORE * (depth + 1);
                }
            }
        }

        // Base cases
        if (depth == 0 || game.board.movesPlayed >= game.board.maxMoves) {
            return evaluatePosition(game);
        }

        int bestScore = -std::numeric_limits<int>::max();
        TTEntry::Type entryType = TTEntry::Type::UPPER_BOUND;

        // Try each possible move using pre-ordered columns
        for (uint16_t col : columnOrder) {
            if (col >= game.board.width) continue;
            if (game.board.canPlace(col)) {
                Game gameCopy(game);
                gameCopy.place(col);
                const int score = -negamax(gameCopy, depth - 1, -beta, -alpha);
                if (score > bestScore) {
                    bestScore = score;
                    if (score > alpha) {
                        alpha = score;
                        entryType = TTEntry::Type::EXACT;
                        if (alpha >= beta) break;
                    }
                }
            }
        }

        // Store position in transposition table
        transpositionTable[state] = {bestScore, depth, entryType};
        return bestScore;
    }

public:
    Solver() {
        // Reserve space for transposition table
        transpositionTable.reserve(1000000);
    }

    int solve(const Game& game, int depth = MAX_DEPTH) {
        nodeCount = 0;
        transpositionTable.clear();  // Clear table for new search
        return negamax(Game(game), depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    }

    [[nodiscard]] unsigned long long getNodeCount() const {
        return nodeCount;
    }
};

uint16_t getMove(Game& game) {
    static Solver solver{};  // Keep solver static to reuse transposition table memory
    std::vector<std::pair<int, uint16_t>> moveScores;
    moveScores.reserve(game.board.width);

    // Check for immediate wins using center-first ordering
    for (uint16_t col : {3, 2, 4, 1, 5, 0, 6}) {
        if (col >= game.board.width) continue;
        if (game.board.canPlace(col)) {
            Game testGame(game);
            if (testGame.place(col)) {
                std::println("Found winning move at column {}", col);
                return col;
            }
        }
    }

    // Evaluate moves in parallel for better performance
    #pragma omp parallel for if(game.board.movesPlayed < 10)
    for (uint16_t col = 0; col < game.board.width; col++) {
        if (game.board.canPlace(col)) {
            Game gameCopy(game);
            gameCopy.place(col);
            int score = -solver.solve(gameCopy);
            #pragma omp critical
            moveScores.emplace_back(score, col);
            std::println("Column {} score: {}", col, score);
        }
    }

    auto bestMove = std::max_element(moveScores.begin(), moveScores.end());
    uint16_t bestCol = bestMove->second;

    std::println("Choosing column {} with score {}", bestCol, bestMove->first);
    std::println("Nodes evaluated: {}", solver.getNodeCount());

    return bestCol;
}