#include "../include/Shell.h"
#include "../include/Direction.h"
#include <iostream>

Shell::Shell(int x, int y, Direction direction, int player_id) : GameObject(x, y), direction(direction), player_id(player_id)  {
}

std::string Shell::getSymbol() const {
    return "*";
}


void Shell::setPosition(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

bool Shell::isShell() const { return true; }
Direction Shell::getDirection() const { return direction; }
int Shell::getPlayerId() const { return player_id; }