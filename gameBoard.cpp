#include "gameBoard.h"
#include <iostream>
#include <iomanip>

//void GameBoard::initializeGrid() {
//    grid.resize(height);
//    for (auto& row : grid) {
//        row.resize(width);
//    }
//}

bool GameBoard::isValidPosition(int x, int y) const {
    //if wall is placed on here should return bad step
    //for input file: wall/mine/tank/
    //
    //not sure if we need to check
    //return x >= 0 && x < height && y >= 0 && y < width;
}

bool GameBoard::placeObject(std::unique_ptr<GameObject> obj, int x, int y) {
    if (!isValidPosition(x, y)) return false;
    if (grid[x][y] != nullptr) return false;
    
    grid[x][y] = std::move(obj);  // Transfer ownership
    grid[x][y]->setPosition(x, y);
    return true;
}

GameObject* GameBoard::getObjectAt(int x, int y) const {
    if (!isValidPosition(x, y)) return nullptr;
    return grid[x][y].get();  // Return raw pointer
}

void GameBoard::removeObject(int x, int y) {
    if (isValidPosition(x, y)) {
        grid[x][y].reset();  // Delete the object
    }
}

// displayBoard() remains the same
// updateBoard() remains the same