#pragma once
#include "GameObject.h"
#include "Direction.h"
#include <utility>

class Shell : public GameObject {
public:
    Shell(int x, int y, Direction direction, int player_id);
    
    std::string getSymbol() const override;
    bool isShell() const override;
    
    Direction getDirection() const;
    int getPlayerId() const;
    void move();
    void setPosition(int new_x, int new_y);
    
private:
    Direction direction;
    int player_id;
};