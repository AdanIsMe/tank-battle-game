#include "../include/GameBoard.h"
#include "../include/Shell.h"
#include "../include/Mine.h"
#include "../include/Wall.h"
#include "../include/Tank.h"
#include <fstream>
#include <sstream>
#include <algorithm>

GameBoard::GameBoard(int width, int height) : width(width), height(height) {}

bool GameBoard::loadFromFile(const std::string& filename, std::string& errors) {
    clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        errors = "Failed to open file: " + filename;
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool hasPlayer1 = false, hasPlayer2 = false;
    
    while (std::getline(file, line)) {
        if (lineNum >= height) break;
        for (int x = 0; x < static_cast<int>(line.size()) && x < width; x++) {
            char c = line[x];
            switch (c) {
                case '1':
                    if (!hasPlayer1) {
                        tanks.push_back(std::make_shared<Tank>(1, x, lineNum, Direction::LEFT));
                        hasPlayer1 = true;
                    } else {
                        errors += "Duplicate player 1 tank at (" + std::to_string(x) + "," + std::to_string(lineNum) + "), ignored.\n";
                    }
                    break;
                case '2':
                    if (!hasPlayer2) {
                        tanks.push_back(std::make_shared<Tank>(2, x, lineNum, Direction::RIGHT));
                        hasPlayer2 = true;
                    } else {
                        errors += "Duplicate player 2 tank at (" + std::to_string(x) + "," + std::to_string(lineNum) + "), ignored.\n";
                    }
                    break;
                case '@':
                    mines.push_back(std::make_shared<Mine>(x, lineNum));
                    break;
                case '#':
                    walls.push_back(std::make_shared<Wall>(x, lineNum));
                    break;
                case ' ':
                    // Empty space, do nothing
                    break;
                default:
                    errors += "Invalid character '" + std::string(1, c) + "' at (" + std::to_string(x) + "," + std::to_string(lineNum) + "), treated as empty space.\n";
                    break;
            }
        }
        lineNum++;
    }
    
    if (!hasPlayer1) errors += "Player 1 tank not found.\n";
    if (!hasPlayer2) errors += "Player 2 tank not found.\n";
    
    file.close();
    return true;
}

std::vector<std::shared_ptr<GameObject>> GameBoard::getObjectsAt(int x, int y) const {
    std::vector<std::shared_ptr<GameObject>> objects;
    
    wrapCoordinates(x, y);
    
    for (const auto& tank : tanks) {
        auto [tx, ty] = tank->getPosition();
        if (tx == x && ty == y) {
            objects.push_back(tank);
        }
    }
    
    for (const auto& shell : shells) {
        auto [sx, sy] = shell->getPosition();
        if (sx == x && sy == y) {
            objects.push_back(shell);
        }
    }
    
    for (const auto& mine : mines) {
        auto [mx, my] = mine->getPosition();
        if (mx == x && my == y) {
            objects.push_back(mine);
        }
    }
    
    for (const auto& wall : walls) {
        auto [wx, wy] = wall->getPosition();
        if (wx == x && wy == y && !wall->isDestroyed()) {
            objects.push_back(wall);
        }
    }
    
    return objects;
}

std::shared_ptr<Tank> GameBoard::getPlayerTank(int playerId) const {
    for (const auto& tank : tanks) {
        if (tank->getPlayerId() == playerId) {
            return tank;
        }
    }
    return nullptr;
}

void GameBoard::addShell(std::shared_ptr<Shell> shell) {
    shells.push_back(shell);
}

void GameBoard::removeShell(std::shared_ptr<Shell> shell) {
    shells.erase(std::remove(shells.begin(), shells.end(), shell), shells.end());
}

void GameBoard::removeWall(std::shared_ptr<Wall> wall) {
    walls.erase(std::remove(walls.begin(), walls.end(), wall), walls.end());
}

void GameBoard::removeMine(std::shared_ptr<Mine> mine) {
    mines.erase(std::remove(mines.begin(), mines.end(), mine), mines.end());
}

void GameBoard::removeTank(std::shared_ptr<Tank> tank) {
    tanks.erase(std::remove(tanks.begin(), tanks.end(), tank), tanks.end());
}

void GameBoard::wrapCoordinates(int& x, int& y) const {
    if (x < 0) x += width;
    if (x >= width) x -= width;
    if (y < 0) y += height;
    if (y >= height) y -= height;
}

void GameBoard::clear() {
    tanks.clear();
    shells.clear();
    mines.clear();
    walls.clear();
}