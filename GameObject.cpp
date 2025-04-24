#include "GameObject.h"

// Constructor now properly initializes all members
GameObject::GameObject(GameBoard& board, const std::string& symbol, int x, int y)
    : board(board), x_coordinate(x), y_coordinate(y), symbol(symbol) {
}

void GameObject::setPosition(int x, int y) {
    // Validate position before setting?
    x_coordinate = x;
    y_coordinate = y;
}

void GameObject::getPosition(int& x, int& y) const {
    x = x_coordinate;
    y = y_coordinate;
}

std::string GameObject::getSymbol() const {
    return symbol;
}
