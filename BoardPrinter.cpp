#include "BoardPrinter.hpp"
#include <iostream>
#include <algorithm>
#include "Board.hpp"

const std::string BoardPrinter::RESET_COLOR = "\033[0m";
const std::string BoardPrinter::BG_WHITE = "\033[47m";

const std::map<int, std::string> BoardPrinter::colors = {
    {0, "\033[30m"},  // black
    {1, "\033[31m"},  // red
    {2, "\033[32m"},  // green
    {3, "\033[33m"},  // yellow
    {4, "\033[34m"},  // blue
    {5, "\033[35m"},  // magenta
    {6, "\033[36m"}   // cyan
};

std::string BoardPrinter::getColorCode(int colorIndex) {
    auto it = colors.find(colorIndex);
    return it != colors.end() ? it->second : colors.at(0);
}

void BoardPrinter::printBoard(const Board& boardInstance, 
                            const std::vector<CellPosition>* win) {
    std::string str;
    const int width = boardInstance.width;
    const int height = boardInstance.height;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int cellValue = boardInstance.board[row * width + col];
            std::string colorCode = getColorCode(cellValue);

            if (!win) {
                str += colorCode + "⬤ " + RESET_COLOR;
            } else {
                bool isWinningCell = std::any_of(
                    win->begin(), 
                    win->end(),
                    [row, col](const CellPosition& cell) {
                        return cell.row == row && cell.col == col;
                    }
                );

                if (isWinningCell) {
                    str += colorCode + BG_WHITE + "⬤" + RESET_COLOR + " ";
                } else {
                    str += colorCode + "⬤ " + RESET_COLOR;
                }
            }
        }
        str += "\n";
    }

    str += "\n";

    std::cout << str;
}