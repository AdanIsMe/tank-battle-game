#include "GameManager.h"
#include <iostream>

GameManager::GameManager(int width, int height) 
    : width(width), height(height) {
    tiles.resize(height, std::vector<Tile>(width));
}

GameManager::~GameManager() {
    // Clean up any remaining entities
}

// Entity Management
void GameManager::addShell(int x, int y, Shell* shell) {
    if (!isValidPosition(x, y)) return;
    tiles[y][x].shells.push_back(shell);
}

void GameManager::removeShell(int x, int y, Shell* shell) {
    if (!isValidPosition(x, y)) return;
    tiles[y][x].shells.remove(shell);
}

void GameManager::addTank(int x, int y, Tank* tank) {
    if (!isValidPosition(x, y)) return;
    tiles[y][x].tanks.insert(tank);
}

void GameManager::removeTank(int x, int y, Tank* tank) {
    if (!isValidPosition(x, y)) return;
    tiles[y][x].tanks.erase(tank);
}

void GameManager::addMine(int x, int y) {
    if (!isValidPosition(x, y)) return;
    tiles[y][x].hasMine = true;
    tiles[y][x].mineDetected = false;
}

void GameManager::addWall(int x, int y) {
    if (!isValidPosition(x, y)) return;
    tiles[y][x].hasWall = true;
    tiles[y][x].wallHits = 2;
}

// Game Logic
void GameManager::update(float deltaTime) {
    detectEntities();
    processCollisions();
    cleanUpDestroyedEntities();
}

void GameManager::detectEntities() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile& tile = tiles[y][x];
            
            // Mine detection
            if (tile.hasMine && !tile.mineDetected) {
                tile.mineDetected = true;
                std::cout << "Mine detected at (" << x << ", " << y << ")\n";
            }

            // Shell detection (processed in insertion order)
            for (Shell* shell : tile.shells) {
                std::cout << "Processing shell at (" << x << ", " << y << ")\n";
            }
        }
    }
}

void GameManager::processCollisions() {
    // Implement your collision logic here
    // Process shell-wall, shell-tank, tank-mine collisions, etc.
}

void GameManager::cleanUpDestroyedEntities() {
    // Remove any destroyed shells, tanks, etc.
}

// Utility
bool GameManager::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void GameManager::printBoardState() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const Tile& tile = tiles[y][x];
            std::cout << "[";
            if (tile.hasWall) std::cout << "W" << tile.wallHits;
            if (tile.hasMine) std::cout << (tile.mineDetected ? "M" : "m");
            std::cout << "s" << tile.shells.size();
            std::cout << "t" << tile.tanks.size();
            std::cout << "] ";
        }
        std::cout << "\n";
    }
}
