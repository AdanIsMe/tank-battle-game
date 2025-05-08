#include "../include/TankAlgorithm.h"
#include <iostream>

// Base implementation is empty as it's an abstract class
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

std::string TankAlgorithm::findSafeDirection(const GameBoard& board, int playerId) const {
    auto myTank = board.getPlayerTank(playerId);
    if (!myTank) return "NONE";
    return "NONE";
}

bool TankAlgorithm::isInDirection(const GameBoard& board, std::pair<int, int> start, std::pair<int, int> end, int dx, int dy) {
    std::cout << "we're in the isInDirection function\n";


    if(start.first == end.first && start.second == end.second) {
        return false; // same position
    }

    if(dx == 0 && dy == 0)
    {
        return false; // no direction
    }

    if(dy == 0) // LEFT or RIGHT
    {
        if(start.second == end.second) {
            return true; // same row
        }
    }

    if(dx == 0) // UP or DOWN
    {
        if(start.first == end.first) {
            return true; // same column
        }
    }

    int x = start.first;  // column
    int y = start.second; // row

    do {
        // Move in direction with wrapping
        x = (x + dx + board.getWidth()) % board.getWidth(); // wrap columns
        y = (y + dy + board.getHeight()) % board.getHeight();   // wrap rows

        if (x == end.first && y == end.second) {
            std::cout << "enemy tank is in my direction\n";
            return true;
        }

    } while (!(x == start.first && y == start.second)); // looped back to start

    return false;
}
