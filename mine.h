#ifndef MINE_H
#define MINE_H

#include <string>
#include "gameObject.h"


class Mine: public GameObject {
    // Mine is a subclass of GameObject
    // It represents a Mine in the game that can be destroyed by tanks(and destroy tanks)

private:
    int x;
    int y;

public:
    //create constrector for wall
    Mine(int x, int y);

};

#endif // MINE_H