#include "../include/TankAlgorithm.h"
#include <iostream>

// Base implementation is empty as it's an abstract class
bool TankAlgorithm::isShellApproaching(const GameBoard& board, const Tank& my_tank) const {
    auto my_pos = my_tank.getPosition();

    for (const auto& shell_ptr : board.getShells()) {
        auto shell_pos = shell_ptr->getPosition();
        Direction shell_dir = shell_ptr->getDirection();
        auto movement = DirectionUtil::getMovement(shell_dir);

        // Horizontal movement
        if (movement.second == 0 && shell_pos.second == my_pos.second) {
            if ((movement.first > 0 && shell_pos.first < my_pos.first) || 
                (movement.first < 0 && shell_pos.first > my_pos.first)) {
                return true;
            }
        }
        // Vertical movement
        else if (movement.first == 0 && shell_pos.first == my_pos.first) {
            if ((movement.second > 0 && shell_pos.second < my_pos.second) || 
                (movement.second < 0 && shell_pos.second > my_pos.second)) {
                return true;
            }
        }
        // Diagonal movement
        else if (std::abs(shell_pos.first - my_pos.first) == std::abs(shell_pos.second - my_pos.second)) {
            int delta_x = my_pos.first - shell_pos.first;
            int delta_y = my_pos.second - shell_pos.second;

            if ((movement.first > 0 && delta_x > 0) || (movement.first < 0 && delta_x < 0)) {
                if ((movement.second > 0 && delta_y > 0) || (movement.second < 0 && delta_y < 0)) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::string TankAlgorithm::findSafeDirection(const GameBoard& board, int player_id) const {
    auto my_tank = board.getPlayerTank(player_id);
    if (!my_tank) return "NONE";
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
