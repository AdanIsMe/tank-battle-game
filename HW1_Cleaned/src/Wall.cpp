#include "../include/Wall.h"

Wall::Wall(int x, int y) : GameObject(x, y) {}

std::string Wall::getSymbol() const {
    return "#";
}

void Wall::hit() {
    hits++;
}

bool Wall::isWall() const { return true; }
bool Wall::isDestroyable() const { return true; }
bool Wall::isDestroyed() const { return hits >= 2; }