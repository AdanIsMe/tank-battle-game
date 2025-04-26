#ifndef CANNON_H
#define CANNON_H

#include <string>
#include "HelperComponents.h"

class Cannon {
private:

    // Cannon's direction (which decides the direction of the shot shell and tanks movement)
    //int x_direction;
    //int y_direction;

    // Cannon's direction - but this time its an enum istead of coordinates
    Direction current_direction;

    // I dont want to define enum direction here because it should be accessible from class shell as well
    /*
    enum Direction {
        UP = 0,
        UP_RIGHT = 1,
        RIGHT = 2,
        DOWN_RIGHT = 3,
        DOWN = 4,
        DOWN_LEFT = 5,
        LEFT = 6,
        UP_LEFT = 7
    };
    */

public:
    // Getters
    Direction getCurrentDirection() const;

    // Actions (maybe later switch them to rotate by degrees)
    void rotate45Degrees(int clockwise_flag); // clockwise_flag = 1 for right, -1 for left

    void rotate90Degrees(int direcclockwise_flagtion); // clockwise_flag = 1 for right, -1 for left



};

#endif // CANNON_H