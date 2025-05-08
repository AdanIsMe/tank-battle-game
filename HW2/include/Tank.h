#pragma once
#include "GameObject.h"
#include "Direction.h"
#include <utility>
#include <vector>

class Tank : public GameObject {
public:
    Tank(int player_id, int x, int y, Direction direction);
    
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
    int player_id;
    Direction direction;
    int shells_left = 16;
    int shoot_cooldown = 0;
    int backward_step = 0; // 0 - not moving, 1-2 - waiting, 3 - moving
};
