#ifndef WALL_H
#define WALL_H

#include <string>
#include "GameObject.h"

class Wall: public GameObject {
    // Wall is a subclass of GameObject
    // It represents a wall in the game that can be destroyed by shells
    // It contains properties and methods specific to walls
private:
    int strength  = 2; // Initlize to 2, so that after 2 hits, the wall will fall.
    int x;
    int y;

public:
    //create constrector for wall
    Wall(int x, int y);
    int getX();
    int getY();
    bool weaken();

};

#endif // WALL_H
