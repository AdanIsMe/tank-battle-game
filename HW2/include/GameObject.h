#pragma once
#include <string>

class GameObject {
public:
    virtual ~GameObject() = default;
    GameObject(int x, int y);

    virtual std::string getSymbol() const = 0;
    virtual bool isCollidable() const;
    virtual bool isDestroyable() const;
    virtual bool isTank() const;
    virtual bool isShell() const;
    virtual bool isMine() const;
    virtual bool isWall() const;
    virtual std::pair<int, int> getPosition() const;

protected:
    int x, y; // Position of the object
};