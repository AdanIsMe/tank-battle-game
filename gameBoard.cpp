#include "gameBoard.h"
#include <iostream>
#include <iomanip>

void GameBoard::initializeGrid() {
    grid.resize(height);
    for (auto& row : grid) {
        row.resize(width, nullptr);
    }
}

bool GameBoard::isValidPosition(int x, int y) const {
    return x >= 0 && x < height && y >= 0 && y < width;
}

bool GameBoard::placeObject(std::shared_ptr<GameObject> obj, int x, int y) {
    if (!isValidPosition(x, y)) return false;
    if (grid[x][y] != nullptr) return false;
    
    grid[x][y] = obj;
    obj->setPosition(x, y);
    return true;
}

std::shared_ptr<GameObject> GameBoard::getObjectAt(int x, int y) const {
    if (!isValidPosition(x, y)) return nullptr;
    return grid[x][y];
}

void GameBoard::removeObject(int x, int y) {
    if (isValidPosition(x, y)) {
        grid[x][y] = nullptr;
    }
}

void GameBoard::displayBoard() const {
    // Top border
    std::cout << "+";
    for (int y = 0; y < width; ++y) {
        std::cout << "---+";
    }
    std::cout << "\n";

    for (int x = 0; x < height; ++x) {
        // Cell content row
        std::cout << "|";
        for (int y = 0; y < width; ++y) {
            auto obj = grid[x][y];
            std::cout << " ";
            if (obj) {
                std::cout << obj->getSymbol();
            } else {
                std::cout << " ";
            }
            std::cout << " |";
        }
        std::cout << "\n";

        // Bottom border
        std::cout << "+";
        for (int y = 0; y < width; ++y) {
            std::cout << "---+";
        }
        std::cout << "\n";
    }
}

void GameBoard::updateBoard(const Cannon& cannon, const Shell& shell) {
    // Implementation depends on your game logic
    // Example: Update cannon and shell positions
    // You might want to make this more generic to handle any game objects
}