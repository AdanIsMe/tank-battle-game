#pragma once
#include <list>
#include <unordered_set>
#include <vector>
#include <memory>

class Shell;

class GameManager {
private:
    std::vector<vector<char>> board;
    int rows;
    int cols;
    int player1TankCount = 0;
    int player2TankCount = 0;
    std::vector<Shell> active_shells;
    std::vector<Tank> tanks;
    int max_steps;
    int num_shells;


public:
    GameManager(int cols, int rows);
    ~GameManager();


private:
    int max_steps;
    const std::vector<std::make_unique<Tank>>& GameManager::getTanks() const { return tanks; }
    const std::vector<std::make_unique<Shell>>& GameManager::getShells() const { return active_shells; }
    bool isValidCellChar(char c) const;
    bool isValidPosition(int x, int y) const;
};
