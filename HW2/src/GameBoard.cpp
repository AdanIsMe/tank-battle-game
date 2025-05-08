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

bool GameBoard::loadFromFile(const std::string& file_name, std::string& errors) {
    (void)errors;
    clear();
    std::ifstream file(file_name);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_name << "\n";
        return false;
    }

    int file_width, file_height;
    file >> file_width >> file_height;

    // Store recoverable warnings
    std::string warnings;

    // Warn if declared dimensions mismatch, but recover
    if (file_width != width || file_height != height) {
        warnings += "Warning: File dimensions (" + std::to_string(file_width) + "x" +
                    std::to_string(file_height) + ") don't match board dimensions (" +
                    std::to_string(width) + "x" + std::to_string(height) + ")\n";
    }

    std::string line;
    std::getline(file, line); // Consume the rest of the dimensions line

    int line_num = 0;
    bool has_player1 = false, has_player2 = false;
    bool has_warnings = false;

    while (std::getline(file, line) && line_num < height) {
        for (int x = 0; x < width; x++) {
            char c = (static_cast<size_t>(x) < line.size()) ? line[x] : ' ';

            if (!isValidCellChar(c)) {
                warnings += "Warning: Invalid character '" + std::string(1, c) +
                            "' at (" + std::to_string(x) + "," + std::to_string(line_num) +
                            "), treated as space\n";
                c = ' ';
                has_warnings = true;
            }

            switch (c) {
                case '1':
                    if (!has_player1) {
                        tanks.push_back(std::make_shared<Tank>(1, x, line_num, Direction::LEFT));
                        has_player1 = true;
                    } else {
                        warnings += "Warning: Duplicate player 1 tank at (" +
                                    std::to_string(x) + "," + std::to_string(line_num) +
                                    "), keeping first occurrence\n";
                        has_warnings = true;
                    }
                    break;
                case '2':
                    if (!has_player2) {
                        tanks.push_back(std::make_shared<Tank>(2, x, line_num, Direction::RIGHT));
                        has_player2 = true;
                    } else {
                        warnings += "Warning: Duplicate player 2 tank at (" +
                                    std::to_string(x) + "," + std::to_string(line_num) +
                                    "), keeping first occurrence\n";
                        has_warnings = true;
                    }
                    break;
                case '#':
                    walls.push_back(std::make_shared<Wall>(x, line_num));
                    break;
                case '@':
                    mines.push_back(std::make_shared<Mine>(x, line_num));
                    break;
                case ' ':
                    break; // empty space
            }
        }
        line_num++;
    }

    // Fill missing rows with spaces
    while (line_num < height) {
        warnings += "Warning: Missing row " + std::to_string(line_num) + ", filled with spaces\n";
        has_warnings = true;
        line_num++;
    }

    // Unrecoverable errors — report to terminal, no input_errors.txt
    if (!has_player1 || !has_player2) {
        return false;
    }

    // Only write warnings to file if there are recoverable issues
    if (has_warnings && !warnings.empty()) {
        std::ofstream error_file("input_errors.txt");
        if (error_file.is_open()) {
            error_file << warnings;
            error_file.close();
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

void GameBoard::setShells(const std::vector<std::shared_ptr<Shell>>& new_shells) {
    shells = new_shells; // This replaces the entire vector
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


void GameBoard::writeBoardToFile(const std::string& file_name) const {
    std::ofstream out_file(file_name);
    if (!out_file) {
        std::cerr << "Error: Unable to open file for writing: " << file_name << std::endl;
        return;
    }

    // Print board dimensions as the first line
    out_file << width << "x" << height << std::endl;

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
        out_file << std::string(grid[y].begin(), grid[y].end()) << std::endl;
    }

    // Save tank directions
    out_file << "Directions:\n";
    for (const auto& tank : tanks) {
        out_file << tank->getPlayerId() << ": " << DirectionUtil::toString(tank->getDirection()) << std::endl;
    }

    out_file.close();
    std::cout << "Game board saved to " << file_name << std::endl;
}

void GameBoard::appendStepToLog(std::ofstream& out_stream, int step_number) const {
    out_stream << "Step " << step_number << ":\n";

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
            out_stream << grid[y][x];
        }
        out_stream << "\n";
    }

    // Output directions
    out_stream << "Directions:\n";
    for (const auto& tank : tanks) {
        out_stream << tank->getPlayerId() << ": "
                  << DirectionUtil::toString(tank->getDirection()) << "\n";
    }

    out_stream << "\n";
}
