#pragma once
#include <vector>
#include <memory>
#include <string>
#include "GameObject.h"

// Forward declarations
class Shell;
class Mine;
class Wall;
class Tank;

class GameBoard {
public:
    GameBoard(int width, int height);
    
    bool loadFromFile(const std::string& file_name, std::string& errors);
    
    void clear();
    int getWidth() const;
    int getHeight() const;
    void writeBoardToFile(const std::string& file_name) const;
    void appendStepToLog(std::ofstream& out_stream, int step_number) const;

    std::vector<std::shared_ptr<GameObject>> getObjectsAt(int x, int y) const;
    std::shared_ptr<Tank> getPlayerTank(int player_id) const;
    const std::vector<std::shared_ptr<Tank>>& getTanks() const;
    const std::vector<std::shared_ptr<Shell>>& getShells() const;
    const std::vector<std::shared_ptr<Mine>>& getMines() const;
    const std::vector<std::shared_ptr<Wall>>& getWalls() const;
    
    void addShell(std::shared_ptr<Shell> shell);
    void removeShell(std::shared_ptr<Shell> shell);
    void removeWall(std::shared_ptr<Wall> wall);
    void removeMine(std::shared_ptr<Mine> mine);
    void removeTank(std::shared_ptr<Tank> tank);
    bool containsShell(const std::shared_ptr<Shell>& shell) const;
    void wrapCoordinates(int& x, int& y) const;
    bool cellHasObstacle(int x, int y) const;
    bool isCellEmpty(int x, int y) const;
    void displayBoard() const;
    void setShells(const std::vector<std::shared_ptr<Shell>>& new_shells);
    bool cellHasWall(int x, int y) const;
    
private:
    int width, height;
    std::vector<std::shared_ptr<Tank>> tanks;
    std::vector<std::shared_ptr<Shell>> shells;
    std::vector<std::shared_ptr<Mine>> mines;
    std::vector<std::shared_ptr<Wall>> walls;

    bool isValidPosition(int x, int y) const;
    bool isValidCellChar(char c) const;
};