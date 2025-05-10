#pragma once

#include <list>
#include <unordered_set>
#include <vector>
#include <memory>

class Shell;
class Tank;

struct Tile {
    bool hasWall = false;
    int wallHits = 2;  // Walls take 2 hits to destroy

    bool hasMine = false;
    bool mineDetected = false;

    std::list<Shell*> shells;          // Shells in insertion order
    std::unordered_set<Tank*> tanks;   // Tanks (order doesn't matter)
};

class GameManager {
public:
    GameManager(int width, int height);
    ~GameManager();

    // Entity management
    void addShell(int x, int y, Shell* shell);
    void removeShell(int x, int y, Shell* shell);
    void addTank(int x, int y, Tank* tank);
    void removeTank(int x, int y, Tank* tank);
    void addMine(int x, int y);
    void addWall(int x, int y);

    // Game logic
    void update(float deltaTime);
    void detectEntities();
    void processCollisions();

    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const Tile& getTile(int x, int y) const { return tiles[y][x]; }

    // Debug
    void printBoardState() const;

private:
    int width, height;
    std::vector<std::vector<Tile>> tiles;

    void cleanUpDestroyedEntities();
    bool isValidPosition(int x, int y) const;
};
