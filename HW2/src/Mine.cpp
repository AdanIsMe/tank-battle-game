#include "../include/Mine.h"

Mine::Mine(int x, int y) : GameObject(x, y) {}

std::string Mine::getSymbol() const {
    return "@";
}

bool Mine::isMine() const { return true; }