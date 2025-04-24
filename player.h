#ifndef PLAYER_H
#define PLAYER_H
#include "gameObject.h"
#include "tank.h"


class Player {
    // player have tanks (for hw1 only one tank)
    // and an algorithm that decide how to play the game based on the state of the board
    private:
    public:
        Tank tank;
        Player(GameManager* gm) : tank(gm,16) {} //create tank object
        Player();
        int nextMove(); // Your existing method
    };
#endif // PLAYER_H

