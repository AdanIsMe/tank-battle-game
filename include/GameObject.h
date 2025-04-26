#pragma once
#include <string>

class GameObject {
public:
    virtual ~GameObject() = default;
    virtual std::string getSymbol() const = 0;
    virtual bool isCollidable() const { return true; }
    virtual bool isDestroyable() const { return false; }
    virtual bool isTank() const { return false; }
    virtual bool isShell() const { return false; }
    virtual bool isMine() const { return false; }
    virtual bool isWall() const { return false; }
};