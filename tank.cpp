#include "tank.h"

// Constructor implementation
Tank::Tank() : num_of_shells(16) {
}

// Method implementation
int Tank::getNumOfShells() const {
    return num_of_shells;
}