#include "../include/TankAlgorithm.h"
// Base implementation is empty as it's an abstract class
//??
bool TankAlgorithm::isShellApproaching(const GameBoard& board, const Tank& myTank) const {
    auto myPos = myTank.getPosition();

    for (const auto& shellPtr : board.getShells()) {
        auto shellPos = shellPtr->getPosition();
        Direction shellDir = shellPtr->getDirection();
        auto movement = DirectionUtil::getMovement(shellDir);

        // Horizontal movement
        if (movement.second == 0 && shellPos.second == myPos.second) {
            if ((movement.first > 0 && shellPos.first < myPos.first) || 
                (movement.first < 0 && shellPos.first > myPos.first)) {
                return true;
            }
        }
        // Vertical movement
        else if (movement.first == 0 && shellPos.first == myPos.first) {
            if ((movement.second > 0 && shellPos.second < myPos.second) || 
                (movement.second < 0 && shellPos.second > myPos.second)) {
                return true;
            }
        }
        // Diagonal movement
        else if (std::abs(shellPos.first - myPos.first) == std::abs(shellPos.second - myPos.second)) {
            int deltaX = myPos.first - shellPos.first;
            int deltaY = myPos.second - shellPos.second;

            if ((movement.first > 0 && deltaX > 0) || (movement.first < 0 && deltaX < 0)) {
                if ((movement.second > 0 && deltaY > 0) || (movement.second < 0 && deltaY < 0)) {
                    return true;
                }
            }
        }
    }
    return false;
}

//??
std::string TankAlgorithm::findSafeDirection(const GameBoard& board, int x, int y, Direction dir) const {
    // Get the movement offsets from DirectionUtil based on the given direction
    std::pair<int, int> directionOffset = DirectionUtil::getMovement(dir);
    
    // Calculating the new position after moving in the direction
    int newX = x + directionOffset.first;
    int newY = y + directionOffset.second;

    // Using the wrapCoordinates method to adjust newX and newY if they are out of bounds
    board.wrapCoordinates(newX, newY);

    // Checking if the new position is free (i.e., there is no object at this position)
    if (board.getObjectsAt(newX, newY).empty()) {
        // Convert the direction to string for output
        return DirectionUtil::toString(dir);  // Return the direction as a string
    }

    return "NONE";  // No safe direction found
}
