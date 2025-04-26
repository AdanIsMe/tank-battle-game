#include "../algorithms/DefensiveAlgorithm.h"


#include <algorithm>

std::string DefensiveAlgorithm::getNextAction(const GameBoard& board, int playerId) {
    auto tank = board.getPlayerTank(playerId);
    if (!tank) return "NONE";
    
    auto [x, y] = tank->getPosition();
    Direction dir = tank->getDirection();
    
    // Check if we're in danger (shell coming towards us)
    if (isShellApproaching(board, x, y, playerId)) {
        std::string safeMove = findSafeDirection(board, x, y, dir, playerId);
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

bool DefensiveAlgorithm::isShellApproaching(const GameBoard& board, int x, int y, int playerId) {
    for (const auto& shell : board.getShells()) {
        if (shell->getPlayerId() != playerId) {
            auto [sx, sy] = shell->getPosition();
            auto [sdx, sdy] = DirectionUtil::getMovement(shell->getDirection());
            
            // Check if shell is heading towards us (within 2 steps)
            if (((sdx != 0 && ((sx - x) * sdx > 0)) && (abs(sy - y) <= 2)) ||
                ((sdy != 0 && ((sy - y) * sdy > 0)) && (abs(sx - x) <= 2))) {
                return true;
            }
        }
    }
    return false;
}

/*
std::string DefensiveAlgorithm::findSafeDirection(const GameBoard& board, int x, int y, Direction currentDir, int playerId) {
    // Try to move backward first
    auto [dx, dy] = DirectionUtil::getMovement(currentDir);
    int bx = x - dx;
    int by = y - dy;
    board.wrapCoordinates(bx, by);
    
    bool canMoveBackward = true;
    for (const auto& obj : board.getObjectsAt(bx, by)) {
        if (obj->isCollidable() && !obj->isShell()) {
            canMoveBackward = false;
            break;
        }
    }
    
    if (canMoveBackward) {
        return "MOVE_BACKWARD";
    }
    
    // Try to rotate left
    Direction leftDir = DirectionUtil::rotateLeft(currentDir);
    auto [ldx, ldy] = DirectionUtil::getMovement(leftDir);
    int lx = x + ldx;
    int ly = y + ldy;
    board.wrapCoordinates(lx, ly);
    
    bool canMoveLeft = true;
    for (const auto& obj : board.getObjectsAt(lx, ly)) {
        if (obj->isCollidable() && !obj->isShell()) {
            canMoveLeft = false;
            break;
        }
    }
    
    if (canMoveLeft) {
        return "ROTATE_LEFT";
    }
    
    // Try to rotate right
    Direction rightDir = DirectionUtil::rotateRight(currentDir);
    auto [rdx, rdy] = DirectionUtil::getMovement(rightDir);
    int rx = x + rdx;
    int ry = y + rdy;
    board.wrapCoordinates(rx, ry);
    
    bool canMoveRight = true;
    for (const auto& obj : board.getObjectsAt(rx, ry)) {
        if (obj->isCollidable() && !obj->isShell()) {
            canMoveRight = false;
            break;
        }
    }
    
    if (canMoveRight) {
        return "ROTATE_RIGHT";
    }
    
    return "NONE";
}
*/

std::string DefensiveAlgorithm::findSafeDirection(const GameBoard& board, int x, int y, Direction currentDir, int playerId) {
    // Get player's tank to avoid considering it as an obstacle
    auto playerTank = board.getPlayerTank(playerId);
    
    // Try to move backward first (most defensive option)
    auto [dx, dy] = DirectionUtil::getMovement(currentDir);
    int bx = x - dx;
    int by = y - dy;
    board.wrapCoordinates(bx, by);
    
    bool canMoveBackward = true;
    for (const auto& obj : board.getObjectsAt(bx, by)) {
        if (obj->isCollidable() && !obj->isShell() && 
            (!playerTank || obj.get() != playerTank.get())) {
            canMoveBackward = false;
            break;
        }
    }
    
    if (canMoveBackward) {
        return "MOVE_BACKWARD";
    }
    
    // Try to rotate left
    Direction leftDir = DirectionUtil::rotateLeft(currentDir);
    auto [ldx, ldy] = DirectionUtil::getMovement(leftDir);
    int lx = x + ldx;
    int ly = y + ldy;
    board.wrapCoordinates(lx, ly);
    
    bool canMoveLeft = true;
    for (const auto& obj : board.getObjectsAt(lx, ly)) {
        if (obj->isCollidable() && !obj->isShell() && 
            (!playerTank || obj.get() != playerTank.get())) {
            canMoveLeft = false;
            break;
        }
    }
    
    if (canMoveLeft) {
        return "ROTATE_LEFT";
    }
    
    // Try to rotate right
    Direction rightDir = DirectionUtil::rotateRight(currentDir);
    auto [rdx, rdy] = DirectionUtil::getMovement(rightDir);
    int rx = x + rdx;
    int ry = y + rdy;
    board.wrapCoordinates(rx, ry);
    
    bool canMoveRight = true;
    for (const auto& obj : board.getObjectsAt(rx, ry)) {
        if (obj->isCollidable() && !obj->isShell() && 
            (!playerTank || obj.get() != playerTank.get())) {
            canMoveRight = false;
            break;
        }
    }
    
    if (canMoveRight) {
        return "ROTATE_RIGHT";
    }
    
    return "NONE";
}