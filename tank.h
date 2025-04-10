#ifndef TANK_H
#define TANK_H

#include <string>
#include "cannon.h"
#include "shell.h"

class Tank {
private:
    Cannon cannon;
    int num_of_shells; // avaliable shells: artillery shells left.
    int position_x;
    int position_y;

public:
    // Constructor
    Tank();

    // Getters
    int getNumOfShells() const;

    // Setters
    void setNumOfShells();

    // Other methods
    void moveForward();
};

#endif // TANK_H