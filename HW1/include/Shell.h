#pragma once
#include "GameObject.h"
#include "Direction.h"
#include <utility>

class Shell : public GameObject {
public:
    Shell(int x, int y, Direction direction, int playerId);
    
    std::string getSymbol() const override;
    bool isShell() const override;
    
    Direction getDirection() const;
    int getPlayerId() const;
    void move();
    void setPosition(int newX, int newY);
    
private:
    Direction direction;
    int playerId;
};