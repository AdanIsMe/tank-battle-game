#include "../include/Wall.h"

Wall::Wall(int x, int y) : x(x), y(y) {}

std::string Wall::getSymbol() const {
    return "#";
}

void Wall::hit() {
    hits++;
}