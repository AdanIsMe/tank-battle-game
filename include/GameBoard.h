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
    
    bool loadFromFile(const std::string& filename, std::string& errors);
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    std::vector<std::shared_ptr<GameObject>> getObjectsAt(int x, int y) const;
    std::shared_ptr<Tank> getPlayerTank(int playerId) const;
    const std::vector<std::shared_ptr<Tank>>& getTanks() const { return tanks; }
    const std::vector<std::shared_ptr<Shell>>& getShells() const { return shells; }
    const std::vector<std::shared_ptr<Mine>>& getMines() const { return mines; }
    const std::vector<std::shared_ptr<Wall>>& getWalls() const { return walls; }
    
    void addShell(std::shared_ptr<Shell> shell);
    void removeShell(std::shared_ptr<Shell> shell);
    void removeWall(std::shared_ptr<Wall> wall);
    void removeMine(std::shared_ptr<Mine> mine);
    void removeTank(std::shared_ptr<Tank> tank);
    
    void wrapCoordinates(int& x, int& y) const;

    void displayBoard() const;
    bool cellHasObstacle(int x, int y) const;
    
private:
    int width, height;
    std::vector<std::shared_ptr<Tank>> tanks;
    std::vector<std::shared_ptr<Shell>> shells;
    std::vector<std::shared_ptr<Mine>> mines;
    std::vector<std::shared_ptr<Wall>> walls;
    
    void clear();
};