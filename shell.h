#ifndef SHELL_H
#define SHELL_H

#include <string>
#include "gameObject.h"

class Shell: public GameObject {
private:
    int x;
    int y;

public:
    void updatePosition();
    int getX();
    int getY();
    bool shouldRemove();
    bool remove = false;
    // Cannon's direction - the shell keeps moving in this direction
    Direction current_direction;
};

#endif // SHELL_H