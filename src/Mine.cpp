#include "../include/Mine.h"

Mine::Mine(int x, int y) : x(x), y(y) {}

std::string Mine::getSymbol() const {
    return "@";
}