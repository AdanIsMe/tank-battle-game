#include "../algorithms/DefensiveAlgorithm.h"


#include <algorithm>

std::string DefensiveAlgorithm::getNextAction(const GameBoard& board, int playerId) {
    auto tank = board.getPlayerTank(playerId);
    if (!tank) return "NONE";
    
    auto [x, y] = tank->getPosition();
    Direction dir = tank->getDirection();
    
    // Check if we're in danger (shell coming towards us)
    if (isShellApproaching(board, *tank)) {
        std::string safeMove = findSafeDirection(board, x, y, dir);
        if (safeMove != "NONE") {
            return safeMove;
        }
    }
    
    // If we can shoot the opponent, do it
    auto opponentTank = board.getPlayerTank(playerId == 1 ? 2 : 1);
    if (opponentTank && tank->getShootCooldown() == 0) {
        auto [ox, oy] = opponentTank->getPosition();
        if ((dir == Direction::LEFT || dir == Direction::RIGHT) && y == oy) {
            if ((dir == Direction::LEFT && x > ox) || (dir == Direction::RIGHT && x < ox)) {
                return "SHOOT";
            }
        } else if ((dir == Direction::UP || dir == Direction::DOWN) && x == ox) {
            if ((dir == Direction::UP && y > oy) || (dir == Direction::DOWN && y < oy)) {
                return "SHOOT";
            }
        }
    }
    
    // Default behavior - move forward if safe
    auto [dx, dy] = DirectionUtil::getMovement(dir);
    int nx = x + dx;
    int ny = y + dy;
    board.wrapCoordinates(nx, ny);
    
    bool canMoveForward = true;
    for (const auto& obj : board.getObjectsAt(nx, ny)) {
        if (obj->isCollidable() && !obj->isShell()) {
            canMoveForward = false;
            break;
        }
    }
    
    if (canMoveForward) {
        return "MOVE_FORWARD";
    }
    
    // If can't move forward, try to rotate
    if (tank->getShootCooldown() == 0) {
        return "ROTATE_RIGHT";
    }
    
    return "NONE";
}

