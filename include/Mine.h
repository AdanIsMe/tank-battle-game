#pragma once
#include "GameObject.h"
#include <utility>

class Mine : public GameObject {
public:
    Mine(int x, int y);
    
    std::string getSymbol() const override;
    bool isMine() const override { return true; }
    
    std::pair<int, int> getPosition() const { return {x, y}; }
    
private:
    int x, y;
};