#include "../include/Shell.h"
#include "../include/Direction.h"

Shell::Shell(int x, int y, Direction direction, int playerId) 
    : x(x), y(y), direction(direction), playerId(playerId) {}

std::string Shell::getSymbol() const {
    return "*";
}

void Shell::move() {
    auto [dx, dy] = DirectionUtil::getMovement(direction);
    x += dx * 2; // Shells move 2 units per step
    y += dy * 2;
}