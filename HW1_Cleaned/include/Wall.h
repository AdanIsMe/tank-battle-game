#pragma once
#include "GameObject.h"
#include <utility>

class Wall : public GameObject {
public:
    Wall(int x, int y);
    
    std::string getSymbol() const override;
    bool isWall() const override;
    bool isDestroyable() const override;
    
    void hit();
    bool isDestroyed() const;
    
private:
    int hits = 0;
};