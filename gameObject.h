#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include "GameBoard.h"

class GameObject {
protected:
    GameBoard& board;  // Reference to the game board
    int x_coordinate;
    int y_coordinate;
    std::string symbol;

public:
    // Constructor now properly initializes all members
    GameObject(GameBoard& board, const std::string& symbol, int x, int y);
    GameObject();
    // Position management
    void setPosition(int x, int y);
    void getPosition(int& x, int& y) const;

    int getX();
    int getY();
    
    // Symbol access
    std::string getSymbol() const;
    
    // Game object actions
    //virtual void update() = 0;  // Pure virtual for polymorphism
    virtual ~GameObject() = default;  // Virtual destructor
};

#endif // GAMEOBJECT_H