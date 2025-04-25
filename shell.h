#ifndef SHELL_H
#define SHELL_H

#include <string>
#include "gameObject.h"

class Shell: public GameObject {
private:
    Direction direction;

public:
    Shell(int x, int y, Direction direction);
    void updatePosition();
    bool shouldRemove();
    bool remove = false;
    // Cannon's direction - the shell keeps moving in this direction
    Direction getDirection() const;
};

#endif // SHELL_H