#include "../include/GameObject.h"

GameObject::GameObject(int x, int y) : x(x), y(y) {}

bool GameObject::isCollidable() const { return true;}
bool GameObject::isDestroyable() const { return false; }
bool GameObject::isTank() const { return false; }
bool GameObject::isShell() const { return false; }
bool GameObject::isMine() const { return false; }
bool GameObject::isWall() const { return false; }

std::pair<int, int> GameObject::getPosition() const { return {x, y}; }