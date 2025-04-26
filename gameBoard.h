#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <vector>
#include <memory>
//#include "cannon.h"
//#include "shell.h"
#include "gameObject.h"
//class GameObject; // Forward declaration of GameObject

class GameBoard {
    private:
        int width;
        int height;
        std::vector<std::vector<std::unique_ptr<GameObject>>> grid;
        

    public:
    
    // Constructor directly initializes the grid to the correct size
    GameBoard(int w, int h);
    GameBoard(GameBoard&& other) noexcept;

    //??????????????????????????
    // Disable copy constructor to prevent copying unique_ptrs
    GameBoard(const GameBoard&) = delete;


    GameBoard& operator=(const GameBoard&) = delete;

    // Move assignment operator
    GameBoard& operator=(GameBoard&& other) noexcept;

    // Getters
    int getWidth() const;
    int getHeight() const;

    // Board operations
    //void initializeGrid();
    bool isValidPosition(int x, int y) const;
    bool placeObject(std::unique_ptr<GameObject>&& obj, int x, int y);  // Changed parameter
    GameObject* getObjectAt(int x, int y) const;  // Changed return type
    void removeObject(int x, int y);
    void addObject(std::unique_ptr<GameObject> object);

    // Game-specific methods
    //void updateBoard(const Cannon& cannon, const Shell& shell);
    //void displayBoard() const;
};

#endif // GAMEBOARD_H