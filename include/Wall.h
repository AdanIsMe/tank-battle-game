#pragma once
#include "GameObject.h"
#include <utility>

class Wall : public GameObject {
public:
    Wall(int x, int y);
    
    std::string getSymbol() const override;
    bool isWall() const override { return true; }
    bool isDestroyable() const override { return true; }
    
    std::pair<int, int> getPosition() const { return {x, y}; }
    void hit();
    bool isDestroyed() const { return hits >= 2; }
    
private:
    int x, y;
    int hits = 0;
};