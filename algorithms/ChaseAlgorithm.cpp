#include "../algorithms/ChaseAlgorithm.h"

#include <queue>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>


std::string ChaseAlgorithm::getNextAction(const GameBoard& board, int playerId) {
    auto tank = board.getPlayerTank(playerId);
    if (!tank) return "NONE";
    
    auto [x, y] = tank->getPosition();
    Direction dir = tank->getDirection();
    
    // Check if we're in danger (shell coming towards us)
    for (const auto& shell : board.getShells()) {
        if (shell->getPlayerId() != playerId) {
            auto [sx, sy] = shell->getPosition();
            auto [sdx, sdy] = DirectionUtil::getMovement(shell->getDirection());
            
            // Simple check if shell is heading towards us
            if ((sdx != 0 && (((sx - x) * sdx) > 0) && (abs(sy - y) <= 1)) ||
                ((sdy != 0 && ((sy - y) * sdy) > 0) && (abs(sx - x) <= 1))) {
                // Try to move away
                if (isPositionSafe(board, x - sdx, y - sdy, playerId)) {
                    return "MOVE_BACKWARD";
                } else if (isPositionSafe(board, x + sdy, y - sdx, playerId)) {
                    return "ROTATE_LEFT";
                } else if (isPositionSafe(board, x - sdy, y + sdx, playerId)) {
                    return "ROTATE_RIGHT";
                }
            }
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
        } else if (dir == Direction::UP_LEFT && (x - ox) == (y - oy) && (x > ox)) {
            return "SHOOT";
        } else if (dir == Direction::UP_RIGHT && (x - ox) == -(y - oy) && (x < ox)) {
            return "SHOOT";
        } else if (dir == Direction::DOWN_LEFT && (x - ox) == -(y - oy) && (x > ox)) {
            return "SHOOT";
        } else if (dir == Direction::DOWN_RIGHT && (x - ox) == (y - oy) && (x < ox)) {
            return "SHOOT";
        }
    }
    
    // Find path to opponent
    auto targetPos = findTargetPosition(board, playerId);
    auto path = findPath(board, {x, y}, targetPos);
    
    if (!path.empty()) {
        auto nextPos = path[0];
        return getMoveDirection({x, y}, nextPos, dir);
    }
    
    // If no path, try to rotate towards opponent
    if (opponentTank) {
        auto [ox, oy] = opponentTank->getPosition();
        if (abs(ox - x) > abs(oy - y)) {
            if (ox > x) {
                if (dir != Direction::RIGHT) return "ROTATE_RIGHT";
            } else {
                if (dir != Direction::LEFT) return "ROTATE_LEFT";
            }
        } else {
            if (oy > y) {
                if (dir != Direction::DOWN) return "ROTATE_RIGHT";
            } else {
                if (dir != Direction::UP) return "ROTATE_LEFT";
            }
        }
    }
    
    return "NONE";
}

std::pair<int, int> ChaseAlgorithm::findTargetPosition(const GameBoard& board, int playerId) {
    auto opponentTank = board.getPlayerTank(playerId == 1 ? 2 : 1);
    if (!opponentTank) return {-1, -1};
    
    auto [ox, oy] = opponentTank->getPosition();
    
    // Try to get to a position near the opponent
    // Prefer positions that are in line with our current direction
    auto tank = board.getPlayerTank(playerId);
    Direction dir = tank->getDirection();
    auto [dx, dy] = DirectionUtil::getMovement(dir);
    
    // Check positions around opponent
    std::vector<std::pair<int, int>> candidates = {
        {ox + 1, oy}, {ox - 1, oy}, {ox, oy + 1}, {ox, oy - 1},
        {ox + 1, oy + 1}, {ox - 1, oy - 1}, {ox + 1, oy - 1}, {ox - 1, oy + 1}
    };
    
    // Filter out unsafe positions
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(), 
        [&](const auto& pos) {
            return !isPositionSafe(board, pos.first, pos.second, playerId);
        }), candidates.end());
    
    if (!candidates.empty()) {
        // Prefer positions that are in our current movement direction
        for (const auto& pos : candidates) {
            if ((pos.first - ox) * dx >= 0 && (pos.second - oy) * dy >= 0) {
                return pos;
            }
        }
        return candidates[0];
    }
    
    return {ox, oy}; // fallback
}

std::vector<std::pair<int, int>> ChaseAlgorithm::findPath(const GameBoard& board, 
                                                        const std::pair<int, int>& start, 
                                                        const std::pair<int, int>& target) {
    if (start == target) return {};
    
    std::queue<std::pair<int, int>> q;
    std::unordered_map<int, std::unordered_map<int, std::pair<int, int>>> parent;
    std::unordered_set<int> visitedX, visitedY;
    
    q.push(start);
    parent[start.first][start.second] = {-1, -1};
    
    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        
        if (x == target.first && y == target.second) {
            // Reconstruct path
            std::vector<std::pair<int, int>> path;
            int cx = x, cy = y;
            while (cx != -1 && cy != -1) {
                path.emplace_back(cx, cy);
                auto [px, py] = parent[cx][cy];
                cx = px;
                cy = py;
            }
            path.pop_back(); // remove start position
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Check all 8 possible directions
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                
                int nx = x + dx;
                int ny = y + dy;
                board.wrapCoordinates(nx, ny);
                
                if (parent[nx].count(ny)) continue;
                
                if (isPositionSafe(board, nx, ny, board.getPlayerTank(1)->getPlayerId())) {
                    parent[nx][ny] = {x, y};
                    q.push({nx, ny});
                }
            }
        }
    }
    
    return {}; // no path found
}

bool ChaseAlgorithm::isPositionSafe(const GameBoard& board, int x, int y, int playerId) {
    board.wrapCoordinates(x, y);
    
    // Check walls
    for (const auto& wall : board.getWalls()) {
        auto [wx, wy] = wall->getPosition();
        if (wx == x && wy == y) return false;
    }
    
    // Check mines
    for (const auto& mine : board.getMines()) {
        auto [mx, my] = mine->getPosition();
        if (mx == x && my == y) return false;
    }
    
    // Check other tanks
    for (const auto& tank : board.getTanks()) {
        if (tank->getPlayerId() == playerId) continue;
        auto [tx, ty] = tank->getPosition();
        if (tx == x && ty == y) return false;
    }
    
    return true;
}

std::string ChaseAlgorithm::getMoveDirection(const std::pair<int, int>& current, 
                                           const std::pair<int, int>& next, 
                                           Direction currentDir) {
    int dx = next.first - current.first;
    int dy = next.second - current.second;
    
    // Handle wrapping
    if (abs(dx) > 1) dx = -dx / abs(dx);
    if (abs(dy) > 1) dy = -dy / abs(dy);
    
    if (dx == 0 && dy == -1) {
        if (currentDir == Direction::UP) return "MOVE_FORWARD";
        if (currentDir == Direction::UP_RIGHT) return "ROTATE_LEFT";
        if (currentDir == Direction::UP_LEFT) return "ROTATE_RIGHT";
        return "ROTATE_LEFT";
    } else if (dx == 1 && dy == -1) {
        if (currentDir == Direction::UP_RIGHT) return "MOVE_FORWARD";
        if (currentDir == Direction::UP) return "ROTATE_RIGHT";
        if (currentDir == Direction::RIGHT) return "ROTATE_LEFT";
        return "ROTATE_LEFT";
    } else if (dx == 1 && dy == 0) {
        if (currentDir == Direction::RIGHT) return "MOVE_FORWARD";
        if (currentDir == Direction::UP_RIGHT) return "ROTATE_RIGHT";
        if (currentDir == Direction::DOWN_RIGHT) return "ROTATE_LEFT";
        return "ROTATE_LEFT";
    } else if (dx == 1 && dy == 1) {
        if (currentDir == Direction::DOWN_RIGHT) return "MOVE_FORWARD";
        if (currentDir == Direction::RIGHT) return "ROTATE_RIGHT";
        if (currentDir == Direction::DOWN) return "ROTATE_LEFT";
        return "ROTATE_LEFT";
    } else if (dx == 0 && dy == 1) {
        if (currentDir == Direction::DOWN) return "MOVE_FORWARD";
        if (currentDir == Direction::DOWN_RIGHT) return "ROTATE_RIGHT";
        if (currentDir == Direction::DOWN_LEFT) return "ROTATE_LEFT";
        return "ROTATE_LEFT";
    } else if (dx == -1 && dy == 1) {
        if (currentDir == Direction::DOWN_LEFT) return "MOVE_FORWARD";
        if (currentDir == Direction::DOWN) return "ROTATE_RIGHT";
        if (currentDir == Direction::LEFT) return "ROTATE_LEFT";
        return "ROTATE_LEFT";
    } else if (dx == -1 && dy == 0) {
        if (currentDir == Direction::LEFT) return "MOVE_FORWARD";
        if (currentDir == Direction::UP_LEFT) return "ROTATE_RIGHT";
        if (currentDir == Direction::DOWN_LEFT) return "ROTATE_LEFT";
        return "ROTATE_LEFT";
    } else if (dx == -1 && dy == -1) {
        if (currentDir == Direction::UP_LEFT) return "MOVE_FORWARD";
        if (currentDir == Direction::UP) return "ROTATE_LEFT";
        if (currentDir == Direction::LEFT) return "ROTATE_RIGHT";
        return "ROTATE_LEFT";
    }
    
    return "NONE";
}