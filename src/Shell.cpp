#include "../include/Shell.h"
#include "../include/Direction.h"

Shell::Shell(int x, int y, Direction direction, int playerId) 
    : x(x), y(y), direction(direction), playerId(playerId) {}

std::string Shell::getSymbol() const {
    return "*";
}

void Shell::move() {
    auto [dx, dy] = DirectionUtil::getMovement(direction);
    x = x + dx; // Shells move 2 units per step
    y = y + dy; //but we check one step at at time
    //x += dx * 2; 
    //y += dy * 2;
}