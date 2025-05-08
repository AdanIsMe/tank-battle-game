#include "../include/Shell.h"
#include "../include/Direction.h"
#include <iostream>

Shell::Shell(int x, int y, Direction direction, int playerId) : GameObject(x, y), direction(direction), playerId(playerId)  {
}

std::string Shell::getSymbol() const {
    return "*";
}


void Shell::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

bool Shell::isShell() const { return true; }
Direction Shell::getDirection() const { return direction; }
int Shell::getPlayerId() const { return playerId; }