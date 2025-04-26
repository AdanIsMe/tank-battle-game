#pragma once
#include "GameObject.h"
#include "Direction.h"
#include <utility>
#include <vector>

class Tank : public GameObject {
public:
    Tank(int playerId, int x, int y, Direction direction);
    
    std::string getSymbol() const override;
    bool isTank() const override { return true; }
    
    int getPlayerId() const { return playerId; }
    std::pair<int, int> getPosition() const { return {x, y}; }
    Direction getDirection() const { return direction; }
    int getShellsLeft() const { return shellsLeft; }
    int getShootCooldown() const { return shootCooldown; }
    int getBackwardMoveStep() const { return backwardMoveStep; }
    
    void setPosition(int newX, int newY);
    void setDirection(Direction newDirection);
    void decreaseShootCooldown();
    void shoot();
    void startBackwardMove();
    //void performBackwardMove();
    void cancelBackwardMove();
    bool isInBackwardMove() const;
    void incraeseWaitTime();
    
private:
    int playerId;
    int x, y;
    Direction direction;
    int shellsLeft = 16;
    int shootCooldown = 0;
    int backwardMoveStep = 0; // 0 - not moving, 1-2 - waiting, 3 - moving
};
