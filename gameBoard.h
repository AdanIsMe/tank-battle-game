#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <string>
#include "cannon.h"
#include "shell.h"

class GameBoard {
private:
    // Game board dimensions
    int width;
    int height;

public:
    // Constructor
    GameBoard(); // Placeholder constructor, not used since we need width and height from the input file
    GameBoard(int w, int h) : width(w), height(h) {}

    // Getters
    int getWidth() const;
    int getHeight() const;

    // Actions
    //void displayBoard() const;
    //void updateBoard(const Cannon& cannon, const Shell& shell);
};

#endif // GAMEBOARD_H