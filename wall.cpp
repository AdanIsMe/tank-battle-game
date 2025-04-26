#include "wall.h"

Wall::Wall(int x, int y)
{
    x = x_coordinate;
    y = y_coordinate;
}


bool Wall::weaken()
{
    strength--;
    return strength == 0;
}

bool Wall::isWall(){
    return true;
}