#pragma once
#include "GameObject.h"
#include <utility>

class Mine : public GameObject {
public:
    Mine(int x, int y);
    
    std::string getSymbol() const override;
    bool isMine() const override;    
};