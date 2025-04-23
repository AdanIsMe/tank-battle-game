#include "wall.h"

Wall::Wall(int x, int y)
{
    x = x;
    y =y;
}

int Wall::getX()
{
    return 0;
}

int Wall::getY()
{
    return 0;
}

bool Wall::weaken()
{
    strength--;
    return strength == 0;
}
