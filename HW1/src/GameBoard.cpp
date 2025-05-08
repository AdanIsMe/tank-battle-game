#include "../include/GameBoard.h"
#include "../include/Shell.h"
#include "../include/Mine.h"
#include "../include/Wall.h"
#include "../include/Tank.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

GameBoard::GameBoard(int width, int height) 
    : width(width), height(height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Board dimensions must be positive");
    }
}

int GameBoard::getWidth() const { return width; }
int GameBoard::getHeight() const { return height; }
const std::vector<std::shared_ptr<Tank>>& GameBoard::getTanks() const { return tanks; }
const std::vector<std::shared_ptr<Shell>>& GameBoard::getShells() const { return shells; }
const std::vector<std::shared_ptr<Mine>>& GameBoard::getMines() const { return mines; }
const std::vector<std::shared_ptr<Wall>>& GameBoard::getWalls() const { return walls; }

bool GameBoard::loadFromFile(const std::string& filename, std::string& errors) {
    (void)errors;
    clear();
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return false;
    }

    int fileWidth, fileHeight;
    file >> fileWidth >> fileHeight;

    // Store recoverable warnings
    std::string warnings;

    // Warn if declared dimensions mismatch, but recover
    if (fileWidth != width || fileHeight != height) {
        warnings += "Warning: File dimensions (" + std::to_string(fileWidth) + "x" +
                    std::to_string(fileHeight) + ") don't match board dimensions (" +
                    std::to_string(width) + "x" + std::to_string(height) + ")\n";
    }

    std::string line;
    std::getline(file, line); // Consume the rest of the dimensions line

    int lineNum = 0;
    bool hasPlayer1 = false, hasPlayer2 = false;
    bool hasWarnings = false;

    while (std::getline(file, line) && lineNum < height) {
        for (int x = 0; x < width; x++) {
            char c = (static_cast<size_t>(x) < line.size()) ? line[x] : ' ';

            if (!isValidCellChar(c)) {
                warnings += "Warning: Invalid character '" + std::string(1, c) +
                            "' at (" + std::to_string(x) + "," + std::to_string(lineNum) +
                            "), treated as space\n";
                c = ' ';
                hasWarnings = true;
            }

            switch (c) {
                case '1':
                    if (!hasPlayer1) {
                        tanks.push_back(std::make_shared<Tank>(1, x, lineNum, Direction::LEFT));
                        hasPlayer1 = true;
                    } else {
                        warnings += "Warning: Duplicate player 1 tank at (" +
                                    std::to_string(x) + "," + std::to_string(lineNum) +
                                    "), keeping first occurrence\n";
                        hasWarnings = true;
                    }
                    break;
                case '2':
                    if (!hasPlayer2) {
                        tanks.push_back(std::make_shared<Tank>(2, x, lineNum, Direction::RIGHT));
                        hasPlayer2 = true;
                    } else {
                        warnings += "Warning: Duplicate player 2 tank at (" +
                                    std::to_string(x) + "," + std::to_string(lineNum) +
                                    "), keeping first occurrence\n";
                        hasWarnings = true;
                    }
                    break;
                case '#':
                    walls.push_back(std::make_shared<Wall>(x, lineNum));
                    break;
                case '@':
                    mines.push_back(std::make_shared<Mine>(x, lineNum));
                    break;
                case ' ':
                    break; // empty space
            }
        }
        lineNum++;
    }

    // Fill missing rows with spaces
    while (lineNum < height) {
        warnings += "Warning: Missing row " + std::to_string(lineNum) + ", filled with spaces\n";
        hasWarnings = true;
        lineNum++;
    }

    // Unrecoverable errors — report to terminal, no input_errors.txt
    if (!hasPlayer1 || !hasPlayer2) {
        return false;
    }

    // Only write warnings to file if there are recoverable issues
    if (hasWarnings && !warnings.empty()) {
        std::ofstream errorFile("input_errors.txt");
        if (errorFile.is_open()) {
            errorFile << warnings;
            errorFile.close();
        }
    }

    return true;
}

bool GameBoard::isValidCellChar(char c) const {
    return c == '1' || c == '2' || c == '#' || c == '@' || c == ' ';
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

// Check if a cell has an obstacle (not a tank)
// Specifically used for checking if tank has a clear path to shoot enemy
bool GameBoard::cellHasObstacle(int x, int y) const
{
    for (const auto& obj : getObjectsAt(x, y)) {
        if (!obj->isTank() && !obj->isMine()) {
            std::cout << "Obstacle detected at (" << x << ", " << y << ")\n";
            return true;
        }
    }
    return false;
}

bool GameBoard::isCellEmpty(int x, int y) const {
    for (const auto& obj : getObjectsAt(x, y)) {
        if (obj->isCollidable()) {
            return false;
        }
    }
    return true;
}


void GameBoard::clear() {
    tanks.clear();
    shells.clear();
    mines.clear();
    walls.clear();
}

void GameBoard::displayBoard() const {
    // Add this at the start of displayBoard()
    std::cout << "\nCurrent Board (" << width << "x" << height << "):\n";
    std::cout << "-------------------------\n";

    // Initialize empty grid with spaces
    std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));

    // Mark walls (indestructible)
    for (const auto& wall : walls) {
        auto [x, y] = wall->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '#';
        }
    }

    // Mark mines
    for (const auto& mine : mines) {
        auto [x, y] = mine->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '@';
        }
    }

    // Mark shells
    for (const auto& shell : shells) {
        auto [x, y] = shell->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = 'o';
        }
    }

    // Mark tanks (will overwrite other objects)
    for (const auto& tank : tanks) {
        auto [x, y] = tank->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '0' + tank->getPlayerId(); // '1' or '2'
        }
    }

    // Print column headers (single-digit only for small boards)
    std::cout << "   ";
    for (int x = 0; x < width; ++x) {
        std::cout << " " << x;
    }
    std::cout << "\n";
    
    // Print grid with row headers
    for (int y = 0; y < height; ++y) {
        std::cout << std::setw(2) << y << " ";
        for (int x = 0; x < width; ++x) {
            std::cout << " " << grid[y][x];
        }
        std::cout << "\n";
    }
    
    // Legend
    std::cout << "\nLegend:\n"
                << "  #: Wall\n"
                << "  @: Mine\n"
                << "  o: Shell\n"
                << "  1: Player 1 Tank\n"
                << "  2: Player 2 Tank\n";

}

bool GameBoard::containsShell(const std::shared_ptr<Shell>& shell) const {
    for (const auto& s : shells) {
        if (s == shell) {
            return true;
        }
    }
    return false;
}

void GameBoard::setShells(const std::vector<std::shared_ptr<Shell>>& newShells) {
    shells = newShells; // This replaces the entire vector
}

bool GameBoard::cellHasWall(int x, int y) const
{
    for (const auto& obj : getObjectsAt(x, y)) {
        if (obj->isWall()) {
            std::cout << "Wall detected at (" << x << ", " << y << ")\n";
            return true;
        }
    }
    return false;
}


void GameBoard::writeBoardToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
        return;
    }

    // Print board dimensions as the first line
    outFile << width << "x" << height << std::endl;

    // Initialize empty grid with spaces
    std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));

    // Mark walls (indestructible)
    for (const auto& wall : walls) {
        auto [x, y] = wall->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '#';
        }
    }

    // Mark mines
    for (const auto& mine : mines) {
        auto [x, y] = mine->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '@';
        }
    }

    // Mark shells
    for (const auto& shell : shells) {
        auto [x, y] = shell->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = 'o';
        }
    }

    // Mark tanks (will overwrite other objects)
    for (const auto& tank : tanks) {
        auto [x, y] = tank->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '0' + tank->getPlayerId(); // '1' or '2'
        }
    }

    // Save grid to file
    for (int y = 0; y < height; ++y) {
        outFile << std::string(grid[y].begin(), grid[y].end()) << std::endl;
    }

    // Save tank directions
    outFile << "Directions:\n";
    for (const auto& tank : tanks) {
        outFile << tank->getPlayerId() << ": " << DirectionUtil::toString(tank->getDirection()) << std::endl;
    }

    outFile.close();
    std::cout << "Game board saved to " << filename << std::endl;
}

void GameBoard::appendStepToLog(std::ofstream& outStream, int stepNumber) const {
    outStream << "Step " << stepNumber << ":\n";

    // Build grid like displayBoard()
    std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));

    for (const auto& wall : walls) {
        auto [x, y] = wall->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height)
            grid[y][x] = '#';
    }

    for (const auto& mine : mines) {
        auto [x, y] = mine->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height)
            grid[y][x] = '@';
    }

    for (const auto& shell : shells) {
        auto [x, y] = shell->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height)
            grid[y][x] = 'o';
    }

    for (const auto& tank : tanks) {
        auto [x, y] = tank->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height)
            grid[y][x] = '0' + tank->getPlayerId();  // '1' or '2'
    }

    // Output grid
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            outStream << grid[y][x];
        }
        outStream << "\n";
    }

    // Output directions
    outStream << "Directions:\n";
    for (const auto& tank : tanks) {
        outStream << tank->getPlayerId() << ": "
                  << DirectionUtil::toString(tank->getDirection()) << "\n";
    }

    outStream << "\n";
}
