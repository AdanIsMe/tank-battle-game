#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include "cannon.h"
#include "shell.h"
#include "GameBoard.h"

class GameObject {
    // GameObject is a base class for all game objects
    // It can be a tank, shell, or any other object in the game
    // It contains common properties and methods for all game objects
protected:

    GameBoard& board;  // Reference to the game board

    // Position on the game board
    int x_coordinate;
    int y_coordinate;


public:
    // Constructor
    GameObject(); // plaveholder, never used since we need width and height from the input file

    // Actions
    //void displayBoard() const;
    //void updateBoard(const Cannon& cannon, const Shell& shell);
};

#endif // GAMEOBJECT_H