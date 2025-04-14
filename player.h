#ifndef PLAYER_H
#define PLAYER_H
#include <string>

#include "cannon.h"
//#include "shell.h"
#include "gameObject.h"
#include "tank.h"

class Player{
    // player have tanks (for hw1 only one tank)
    // and an algorthem that decide how to play the game based on the state of the board
private:
    
public:
    // Constructor
    bool my_shells = true;
    Tank myTank;
    Player();
    int nextMove();
};

#endif // PLAYER_H
