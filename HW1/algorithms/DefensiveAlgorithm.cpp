#include "../algorithms/DefensiveAlgorithm.h"
#include <algorithm>
#include <iostream>

std::string DefensiveAlgorithm::getNextAction(const GameBoard& board, int playerId) {
    auto tank = board.getPlayerTank(playerId);
    if (!tank) return "NONE";
    
    auto [x, y] = tank->getPosition();
    
    // 1. Immediate danger handling (shells)
    if (isInImmediateDanger(board, x, y)) {
        std::string dangerResponse = handleImmediateDanger(board, tank);
        if (dangerResponse != "NONE") {
            return dangerResponse;
        }
    }
    
    // 2. Safe offensive opportunity
    if (canSafelyShootOpponent(board, tank)) {
        return "SHOOT";
    }
    
    //3. Default safe movement
    return getDefaultSafeMove(board, tank);
}

bool DefensiveAlgorithm::isInImmediateDanger(const GameBoard& board, int x, int y) {
    for (const auto& shell : board.getShells()) {

        auto [sx, sy] = shell->getPosition();
        Direction shellDir = shell->getDirection();
        auto [dx, dy] = DirectionUtil::getMovement(shellDir);
        
        // Calculate both movement steps (2 tiles per turn)
        int nextX1 = sx + dx;
        int nextY1 = sy + dy;
        board.wrapCoordinates(nextX1, nextY1);
        
        int nextX2 = nextX1 + dx;
        int nextY2 = nextY1 + dy;
        board.wrapCoordinates(nextX2, nextY2);
        
        // Check if we're in either step's path
        if ((nextX1 == x && nextY1 == y) || (nextX2 == x && nextY2 == y)) {
            return true;
        }
    }
    return false;
}

std::string DefensiveAlgorithm::handleImmediateDanger(const GameBoard& board, std::shared_ptr<Tank> tank) {
    auto [x, y] = tank->getPosition();
    Direction dir = tank->getDirection();
    int playerId = tank->getPlayerId();
    
    // 1. Try to shoot the incoming shell if possible
    if (tank->getShootCooldown() == 0 && canShootShell(board, x, y, dir, playerId)) {
        return "SHOOT";
    }
    
    // Calculate forward movement position
    auto [fdx, fdy] = DirectionUtil::getMovement(dir);
    int forwardX = x + fdx;
    int forwardY = y + fdy;
    board.wrapCoordinates(forwardX, forwardY);
    
    // 2. Check if moving forward would help avoid danger
    bool forwardIsSafe = isTileSafe(board, forwardX, forwardY);
    bool forwardAvoidsShell = false;
    
    // Check all threatening shells
    for (const auto& shell : board.getShells()) {
       /// if (shell->getPlayerId() == playerId) continue;
        
        auto [sx, sy] = shell->getPosition();
        Direction shellDir = shell->getDirection();
        auto [sdx, sdy] = DirectionUtil::getMovement(shellDir);
        
        // Calculate shell's next positions (accounting for 2-tile movement)
        int shellX1 = sx + sdx;
        int shellY1 = sy + sdy;
        board.wrapCoordinates(shellX1, shellY1);
        
        int shellX2 = shellX1 + sdx;
        int shellY2 = shellY1 + sdy;
        board.wrapCoordinates(shellX2, shellY2);
        
        // Check if moving forward would take us away from shell path
        bool forwardInDanger = (forwardX == shellX1 && forwardY == shellY1) || 
                              (forwardX == shellX2 && forwardY == shellY2);
        
        if (!forwardInDanger) {
            forwardAvoidsShell = true;
            break;
        }
    }
    
    // If forward movement both safe and avoids shells, prioritize it
    if (forwardIsSafe && forwardAvoidsShell) {
        return "MOVE_FORWARD";
    }
    
    // 3. Last resort options
    if (forwardIsSafe) {
        return "MOVE_FORWARD";  // Even if it doesn't avoid shells, better than nothing
    }
    
    // Try backward move as absolute last resort
    int backwardX = x - fdx;
    int backwardY = y - fdy;
    board.wrapCoordinates(backwardX, backwardY);
    
    if (isTileSafe(board, backwardX, backwardY)) {
        return "MOVE_BACKWARD";
    }
    
    // If completely trapped, just rotate to potentially face threats
    return "ROTATE_RIGHT";
}

bool DefensiveAlgorithm::canShootShell(const GameBoard& board, int x, int y, Direction dir, int playerId) {
    for (const auto& shell : board.getShells()) {
        if (shell->getPlayerId() == playerId) continue;
        
        auto [sx, sy] = shell->getPosition();
        if (dir == Direction::LEFT && sy == y && sx < x) return true;
        if (dir == Direction::RIGHT && sy == y && sx > x) return true;
        if (dir == Direction::UP && sx == x && sy < y) return true;
        if (dir == Direction::DOWN && sx == x && sy > y) return true;
    }
    return false;
}

bool DefensiveAlgorithm::canSafelyShootOpponent(const GameBoard& board, std::shared_ptr<Tank> tank) {
    if (tank->getShootCooldown() > 0) return false;
    
    auto opponent = board.getPlayerTank(tank->getPlayerId() == 1 ? 2 : 1);
    if (!opponent) return false;
    
    auto [x, y] = tank->getPosition();
    auto [ox, oy] = opponent->getPosition();
    Direction dir = tank->getDirection();
    
    // Check shooting lines (including diagonals)
    if ((dir == Direction::LEFT || dir == Direction::RIGHT) && y == oy) {
        if ((dir == Direction::LEFT && x > ox) || (dir == Direction::RIGHT && x < ox)) {
            return hasClearShot(board, x, y, ox, oy);
        }
    } 
    else if ((dir == Direction::UP || dir == Direction::DOWN) && x == ox) {
        if ((dir == Direction::UP && y > oy) || (dir == Direction::DOWN && y < oy)) {
            return hasClearShot(board, x, y, ox, oy);
        }
    }
    else if ((dir == Direction::UP_RIGHT || dir == Direction::DOWN_LEFT ||
              dir == Direction::DOWN_RIGHT || dir == Direction::UP_LEFT) &&
             (std::abs(x - ox) == std::abs(y - oy))) {
        if ((dir == Direction::UP_RIGHT && ox > x && oy < y) ||
            (dir == Direction::DOWN_RIGHT && ox > x && oy > y) ||
            (dir == Direction::DOWN_LEFT && ox < x && oy > y) ||
            (dir == Direction::UP_LEFT && ox < x && oy < y)) {
            return hasClearShot(board, x, y, ox, oy);
        }
    }
    
    return false;
}

bool DefensiveAlgorithm::hasClearShot(const GameBoard& board, int fromX, int fromY, int toX, int toY) { 
    // Horizontal shot
    if (fromY == toY) {
        int step = (fromX < toX) ? 1 : -1;
        for (int x = fromX + step; x != toX; x += step) {
            for (const auto& obj : board.getObjectsAt(x, fromY)) {
                if (!obj->isTank() || !obj->isMine()) {
                    return false;
                }
            }
        }
    }
    // Vertical shot
    else if (fromX == toX) {
        int step = (fromY < toY) ? 1 : -1;
        for (int y = fromY + step; y != toY; y += step) {
            for (const auto& obj : board.getObjectsAt(fromX, y)) {
                if (!obj->isTank() || !obj->isMine()) {
                    //std::cout << "Obstacle detected at (" << fromX << ", " << y << ")\n";
                    return false;
                }
            }
        }
    }
    // Diagonal shot
    else {
        int dx = (fromX < toX) ? 1 : -1;
        int dy = (fromY < toY) ? 1 : -1;
        int x = fromX + dx;
        int y = fromY + dy;
        
        while (x != toX && y != toY) {
            for (const auto& obj : board.getObjectsAt(x, y)) {
                if (!obj->isTank() || !obj->isMine()) {
                    //std::cout << "Obstacle detected at (" << x << ", " << y << ")\n";
                    return false;
                }
            }
            x += dx;
            y += dy;
        }
    }
    return true;
}

std::string DefensiveAlgorithm::getDefaultSafeMove(const GameBoard& board, std::shared_ptr<Tank> tank) {

    auto [x, y] = tank->getPosition();
    Direction dir = tank->getDirection();

    auto enemyTank = board.getPlayerTank(tank->getPlayerId() == 1 ? 2 : 1);
    Direction enemyDir = enemyTank->getDirection();
    auto [direction_x, direction_y] = DirectionUtil::getMovement(enemyDir);

    if (isInDirection(board, (*enemyTank).getPosition(), (*tank).getPosition(), direction_x, direction_y)) {
         // Try forward movement first
        auto [dx, dy] = DirectionUtil::getMovement(dir);
        int nx = x + dx;
        int ny = y + dy;
        board.wrapCoordinates(nx, ny);
        
        if (isTileSafe(board, nx, ny)) {
            return "MOVE_FORWARD";
        }
    }
    else{
        // Check if we can rotate to a safer direction
        // Try forward movement first
        auto [dx, dy] = DirectionUtil::getMovement(dir);
        int nx = x + dx;
        int ny = y + dy;
        board.wrapCoordinates(nx, ny);
                 
        if (isTileSafe(board, nx, ny)) {
            return "MOVE_FORWARD";
        }
        for (const auto& newDir : DirectionUtil::allDirections()) {
            auto [dx, dy] = DirectionUtil::getMovement(newDir);
            int nx = x + dx;
            int ny = y + dy;
            board.wrapCoordinates(nx, ny);
            bool isSafe = false;
            
            if (isTileSafe(board, nx, ny)) {
                isSafe = true;;
            }
            if(isSafe){
                if (DirectionUtil::rotateLeft(dir) == newDir) {
                    return "ROTATE_LEFT";
                } else if (DirectionUtil::rotateRight(dir) == newDir) {
                    return "ROTATE_RIGHT";
                } else if (DirectionUtil::rotateLeftQuarter(dir) == newDir) {
                    return "ROTATE_LEFT_QUARTER";
                } else if (DirectionUtil::rotateRightQuarter(dir) == newDir) {
                    return "ROTATE_RIGHT_QUARTER";
                }
            }
        }
    }
    return "NONE";   
}

bool DefensiveAlgorithm::isTileSafe(const GameBoard& board, int tx, int ty) {
    return (board.isCellEmpty(tx, ty));
}