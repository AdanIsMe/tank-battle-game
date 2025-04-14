#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <vector>
#include <memory>
#include "cannon.h"
#include "shell.h"
#include "gameObject.h"

class GameBoard {
private:
    int width;
    int height;
    std::vector<std::vector<std::shared_ptr<GameObject>>> grid;

public:
    // Constructors
    GameBoard() : width(0), height(0) {}  // Default constructor
    GameBoard(int w, int h) : width(w), height(h) { initializeGrid(); }

    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Board operations
    void initializeGrid();
    bool isValidPosition(int x, int y) const;
    bool placeObject(std::shared_ptr<GameObject> obj, int x, int y);
    std::shared_ptr<GameObject> getObjectAt(int x, int y) const;
    void removeObject(int x, int y);

    // Game-specific methods
    void updateBoard(const Cannon& cannon, const Shell& shell);
    void displayBoard() const;
};

#endif // GAMEBOARD_H