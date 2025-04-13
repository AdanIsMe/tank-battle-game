#ifndef WALL_H
#define WALL_H

#include <string>
#include "gameObject.h"

class Wall: public GameObject {
    // Wall is a subclass of GameObject
    // It represents a wall in the game that can be destroyed by shells
    // It contains properties and methods specific to walls
private:
    int strength; // Initlize to 2, so that after 2 hits, the wall will fall.

public:

};

#endif // WALL_H