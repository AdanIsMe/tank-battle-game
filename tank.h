#ifndef TANK_H
#define TANK_H

#include <string>

#include "cannon.h"
//#include "shell.h"
#include "gameObject.h"

class Tank: public GameObject {
    // Tank is a subclass of GameObject
    // It represents a tank in the game that can shoot shells
    // It contains properties and methods specific to tanks
    // It also contains a cannon object, which is a part of the tank
private:
    
    Cannon cannon;
    int num_of_shells; // avaliable shells: artillery shells left.
    int waited = 0; //keep track of steps waiting(2 steps)
    int shoot_cnt = 0; //keep track of game steps(can't shoot again for 4 steps)

    // Tanks position on the game board
    int x_coordinate;
    int y_coordinate;

public:
    // Constructor
    Tank();

    bool isAlive = true;
    // Getters
    int getNumOfShells() const;

    // Setters - might not be needed
    void setNumOfShells();

    // Actions
    void action(int command);
    void shoot();
    void moveForward();
    void moveBackward();
    

    //so its,,,, rotating cannons not tanks, so why put it here? move to class cannon
    //void rotateCannonByEighth(int direction); // 45 degrees: 1 for right, -1 for left
    //void rotateCannonByQuarter(int direction); // 90 degrees: 1 for right, -1 for left

    //void shoot();
};

#endif // TANK_H