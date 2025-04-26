#pragma once
#include "GameObject.h"
#include "Direction.h"
#include <utility>

class Shell : public GameObject {
public:
    Shell(int x, int y, Direction direction, int playerId);
    
    std::string getSymbol() const override;
    bool isShell() const override { return true; }
    
    std::pair<int, int> getPosition() const { return {x, y}; }
    Direction getDirection() const { return direction; }
    int getPlayerId() const { return playerId; }
    void move();
    
private:
    int x, y;
    Direction direction;
    int playerId;
};