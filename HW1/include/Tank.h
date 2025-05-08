#pragma once
#include "GameObject.h"
#include "Direction.h"
#include <utility>
#include <vector>

class Tank : public GameObject {
public:
    Tank(int playerId, int x, int y, Direction direction);
    
    std::string getSymbol() const override;
    bool isTank() const override;
    
    int getPlayerId() const;
    Direction getDirection() const;
    int getShellsLeft() const;
    int getShootCooldown() const;
    int getBackwardMoveStep() const;
    
    void setPosition(int newX, int newY);
    void setDirection(Direction newDirection);
    void decreaseShootCooldown();
    void shoot();
    void startBackwardMove();
    void cancelBackwardMove();
    bool isInBackwardMove() const;
    void incraeseWaitTime();
    
private:
    int playerId;
    Direction direction;
    int shellsLeft = 16;
    int shootCooldown = 0;
    int backwardMoveStep = 0; // 0 - not moving, 1-2 - waiting, 3 - moving
};
