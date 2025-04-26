#include "gameBoard.h"
#include <iostream>
#include <iomanip>

//void GameBoard::initializeGrid() {
//    grid.resize(height);
//    for (auto& row : grid) {
//        row.resize(width);
//    }
//}

// Constructor directly initializes the grid to the correct size
//GameBoard::GameBoard(int w, int h) : width(w), height(h), grid(h, std::vector<std::unique_ptr<GameObject>>(w)) {}

GameBoard::GameBoard(int w, int h) 
    : width(w), height(h), grid(h, std::vector<std::unique_ptr<GameObject>>{}) {
    for (int i = 0; i < h; ++i) {
        grid[i].resize(w); // initialize each row with 'w' empty unique_ptrs
    }
}



// Move Constructor
GameBoard::GameBoard(GameBoard&& other) noexcept
    : width(other.width), height(other.height), grid(std::move(other.grid)) {
    // Other resources will be moved automatically
    other.width = 0;
    other.height = 0;
}

// Getters
int GameBoard::getWidth() const { return width; }
int GameBoard::getHeight() const { return height; }

bool GameBoard::isValidPosition(int x, int y) const {
    //if wall is placed on here should return bad step
    //for input file: wall/mine/tank/
    //
    //not sure if we need to check
    //return x >= 0 && x < height && y >= 0 && y < width;
    return x >= 0 && x < height && y >= 0 && y < width; // Check bounds
}


// Move Assignment Operator
GameBoard& GameBoard::operator=(GameBoard&& other) noexcept {
    if (this != &other) {
        width = other.width;
        height = other.height;
        grid = std::move(other.grid);  // Move grid
        other.width = 0;
        other.height = 0;
    }
    return *this;
}

// Destructor
//GameBoard::~GameBoard() {
    // Destructor will be automatically called for the unique_ptrs in grid
//}

// Add game object to the grid
void GameBoard::addObject(std::unique_ptr<GameObject> object) {
    // For example, you may want to place the object in a specific position
    // Assume you know where to place it (x, y)
    int x = 0, y = 0;  // This would be based on some logic
    grid[y][x] = std::move(object);  // Move the object into the grid
}


bool GameBoard::placeObject(std::unique_ptr<GameObject>&& obj, int x, int y) {
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