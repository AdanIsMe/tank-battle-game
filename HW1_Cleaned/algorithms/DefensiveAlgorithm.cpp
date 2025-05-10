#include "../algorithms/DefensiveAlgorithm.h"
#include <algorithm>
#include <iostream>

std::string DefensiveAlgorithm::getNextAction(const GameBoard& board, int player_id) {
    auto tank = board.getPlayerTank(player_id);
    if (!tank) return "NONE";
    
    auto [x, y] = tank->getPosition();
    
    // 1. Immediate danger handling (shells)
    if (isInImmediateDanger(board, x, y)) {
        std::string danger_response = handleImmediateDanger(board, tank);
        if (danger_response != "NONE") {
            return danger_response;
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
        Direction shell_dir = shell->getDirection();
        auto [dx, dy] = DirectionUtil::getMovement(shell_dir);
        
        // Calculate both movement steps (2 tiles per turn)
        int next_x1 = sx + dx;
        int next_y1 = sy + dy;
        board.wrapCoordinates(next_x1, next_y1);
        
        int next_x2 = next_x1 + dx;
        int next_y2 = next_y1 + dy;
        board.wrapCoordinates(next_x2, next_y2);
        
        // Check if we're in either step's path
        if ((next_x1 == x && next_y1 == y) || (next_x2 == x && next_y2 == y)) {
            return true;
        }
    }
    return false;
}

std::string DefensiveAlgorithm::handleImmediateDanger(const GameBoard& board, std::shared_ptr<Tank> tank) {
    auto [x, y] = tank->getPosition();
    Direction dir = tank->getDirection();
    int player_id = tank->getPlayerId();
    
    // 1. Try to shoot the incoming shell if possible
    if (tank->getShootCooldown() == 0 && canShootShell(board, x, y, dir, player_id)) {
        return "SHOOT";
    }
    
    // Calculate forward movement position
    auto [fdx, fdy] = DirectionUtil::getMovement(dir);
    int forward_x = x + fdx;
    int forward_y = y + fdy;
    board.wrapCoordinates(forward_x, forward_y);
    
    // 2. Check if moving forward would help avoid danger
    bool forward_is_safe = isTileSafe(board, forward_x, forward_y);
    bool forward_avoids_shell = false;
    
    // Check all threatening shells
    for (const auto& shell : board.getShells()) {
       /// if (shell->getPlayerId() == playerId) continue;
        
        auto [sx, sy] = shell->getPosition();
        Direction shell_dir = shell->getDirection();
        auto [sdx, sdy] = DirectionUtil::getMovement(shell_dir);
        
        // Calculate shell's next positions (accounting for 2-tile movement)
        int shell_x1 = sx + sdx;
        int shell_y1 = sy + sdy;
        board.wrapCoordinates(shell_x1, shell_y1);
        
        int shell_x2 = shell_x1 + sdx;
        int shell_y2 = shell_y1 + sdy;
        board.wrapCoordinates(shell_x2, shell_y2);
        
        // Check if moving forward would take us away from shell path
        bool forward_in_danger = (forward_x == shell_x1 && forward_y == shell_y1) || 
                              (forward_x == shell_x2 && forward_y == shell_y2);
        
        if (!forward_in_danger) {
            forward_avoids_shell = true;
            break;
        }
    }
    
    // If forward movement both safe and avoids shells, prioritize it
    if (forward_is_safe && forward_avoids_shell) {
        return "MOVE_FORWARD";
    }
    
    // 3. Last resort options
    if (forward_is_safe) {
        return "MOVE_FORWARD";  // Even if it doesn't avoid shells, better than nothing
    }
    
    // Try backward move as absolute last resort
    int backward_x = x - fdx;
    int backward_y = y - fdy;
    board.wrapCoordinates(backward_x, backward_y);
    
    if (isTileSafe(board, backward_x, backward_y)) {
        return "MOVE_BACKWARD";
    }
    
    // If completely trapped, just rotate to potentially face threats
    return "ROTATE_RIGHT";
}

bool DefensiveAlgorithm::canShootShell(const GameBoard& board, int x, int y, Direction dir, int player_id) {
    for (const auto& shell : board.getShells()) {
        if (shell->getPlayerId() == player_id) continue;
        
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

bool DefensiveAlgorithm::hasClearShot(const GameBoard& board, int from_x, int from_y, int to_x, int to_y) { 
    // Horizontal shot
    if (from_y == to_y) {
        int step = (from_x < to_x) ? 1 : -1;
        for (int x = from_x + step; x != to_x; x += step) {
            for (const auto& obj : board.getObjectsAt(x, from_y)) {
                if (!obj->isTank() || !obj->isMine()) {
                    return false;
                }
            }
        }
    }
    // Vertical shot
    else if (from_x == to_x) {
        int step = (from_y < to_y) ? 1 : -1;
        for (int y = from_y + step; y != to_y; y += step) {
            for (const auto& obj : board.getObjectsAt(from_x, y)) {
                if (!obj->isTank() || !obj->isMine()) {
                    //std::cout << "Obstacle detected at (" << from_x << ", " << y << ")\n";
                    return false;
                }
            }
        }
    }
    // Diagonal shot
    else {
        int dx = (from_x < to_x) ? 1 : -1;
        int dy = (from_y < to_y) ? 1 : -1;
        int x = from_x + dx;
        int y = from_y + dy;
        
        while (x != to_x && y != to_y) {
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

    auto enemy_tank = board.getPlayerTank(tank->getPlayerId() == 1 ? 2 : 1);
    Direction enemy_dir = enemy_tank->getDirection();
    auto [direction_x, direction_y] = DirectionUtil::getMovement(enemy_dir);

    if (isInDirection(board, (*enemy_tank).getPosition(), (*tank).getPosition(), direction_x, direction_y)) {
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
        for (const auto& new_dir : DirectionUtil::allDirections()) {
            auto [dx, dy] = DirectionUtil::getMovement(new_dir);
            int nx = x + dx;
            int ny = y + dy;
            board.wrapCoordinates(nx, ny);
            bool is_safe = false;
            
            if (isTileSafe(board, nx, ny)) {
                is_safe = true;;
            }
            if(is_safe){
                if (DirectionUtil::rotateLeft(dir) == new_dir) {
                    return "ROTATE_LEFT";
                } else if (DirectionUtil::rotateRight(dir) == new_dir) {
                    return "ROTATE_RIGHT";
                } else if (DirectionUtil::rotateLeftQuarter(dir) == new_dir) {
                    return "ROTATE_LEFT_QUARTER";
                } else if (DirectionUtil::rotateRightQuarter(dir) == new_dir) {
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