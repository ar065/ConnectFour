#pragma once
#include <string>
#include <vector>
#include <map>
#include "Board.hpp"

class Board;  // Forward declaration, assuming Board class exists

class BoardPrinter {
public:
    static void printBoard(const Board& boardInstance, 
                         const std::vector<CellPosition>* win = nullptr);

private:
    static const std::map<int, std::string> colors;
    static std::string getColorCode(int colorIndex);
    static const std::string RESET_COLOR;
    static const std::string BG_WHITE;
};