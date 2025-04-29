#include "../algorithms/OffensiveAlgorithm.h"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <iostream> // For print statements

std::string OffensiveAlgorithm::getEvasionAction(const GameBoard& board, const Tank& tank) const
{
    //auto tank = board.getPlayerTank(playerId);
    //if (!tank) return "NONE";
    
    auto [x, y] = tank.getPosition();
    Direction dir = tank.getDirection();
    
    // Check if we're in danger (shell coming towards us)
    if (isShellApproaching(board, tank)) {
        std::string safeMove = findSafeDirection(board, x, y, dir); // from tankAlgorithm
        if (safeMove != "NONE") {
            return safeMove;
        }
    }
    
    return "NONE"; // No evasion needed
}

// Check if enemy tank is in line of sight
bool OffensiveAlgorithm::enemyInSight(const GameBoard& board, const Tank& myTank, const Tank& enemyTank) {
    std::pair<int, int> myPos = myTank.getPosition();
    std::pair<int, int> enemyPos = enemyTank.getPosition();
    auto [direction_x, direction_y] = DirectionUtil::getMovement(myTank.getDirection());

    // Check if enemy is generally in our facing direction
    if (!isInDirection(board, myPos, enemyPos, direction_x, direction_y)) {
        return false;
    }

    // Check each cell along the line for obstacles
    return hasClearPath(board, myPos, enemyPos);
}


bool OffensiveAlgorithm::isInDirection(const GameBoard& board, std::pair<int, int> start, std::pair<int, int> end, int dx, int dy) {
    int width = board.getWidth();
    int height = board.getHeight();
    int rel_x = end.first - start.first;
    int rel_y = end.second - start.second;

    // Handle board wrapping if needed
    if (abs(rel_x) > width / 2) {
        rel_x = rel_x > 0 ? rel_x - width : rel_x + width;
    }
    if (abs(rel_y) > height / 2) {
        rel_y = rel_y > 0 ? rel_y - height : rel_y + height;
    }
    
    // If the direction is purely horizontal or vertical (UP, RIGHT, DOWN, LEFT)
    if (dx == 0) {
        if (rel_x != 0) return false;         // Must stay on the same x
        return (rel_y * dy > 0);               // Must move in the dy direction
    }
    if (dy == 0) {
        if (rel_y != 0) return false;          // Must stay on the same y
        return (rel_x * dx > 0);               // Must move in the dx direction
    }

    // Diagonal directions
    // Check general direction
    if (rel_x * dx <= 0 || rel_y * dy <= 0) return false;

    // Check if aligned diagonally (slope match)
    return (rel_x * dy == rel_y * dx);
}


bool OffensiveAlgorithm::hasClearPath(const GameBoard& board, std::pair<int, int> start, std::pair<int, int> end) {
    int x0 = start.first;
    int y0 = start.second;
    int x1 = end.first;
    int y1 = end.second;

    int dx = x1 - x0;
    int dy = y1 - y0;

    int step_x = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int step_y = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

    int board_width = board.getWidth();
    int board_height = board.getHeight();

    // Normalize deltas
    dx = abs(dx);
    dy = abs(dy);

    int steps = std::max(dx, dy);

    for (int i = 1; i < steps; ++i) {  // i=1: skip starting position
        int curr_x = x0 + i * step_x;
        int curr_y = y0 + i * step_y;

        // Handle wrapping around the board
        curr_x = (curr_x + board_width) % board_width;
        curr_y = (curr_y + board_height) % board_height;

        // Check if a cell on the path has an object that blocks shell from reaching the enemy
        if (board.cellHasObstacle(curr_x, curr_y)) {
            return false;  // obstacle in the way
        }
    }

    return true; // Clear path!
}


// Checks if the shoot cooldown elapsed
bool OffensiveAlgorithm::canShootNow(const Tank& tank) {
    return (tank.getShootCooldown() == 0);
}


Direction OffensiveAlgorithm::getBestRotationToEnemy(const GameBoard& board, const Tank& myTank, const Tank& enemyTank) const {
    const std::pair<int, int> myPos = myTank.getPosition();
    const std::pair<int, int> enemyPos = enemyTank.getPosition();
    //const Direction currentDir = myTank.getDirection();

    int dx = enemyPos.first - myPos.first;
    int dy = enemyPos.second - myPos.second;

    // Adjusting wrapping
    if (std::abs(dx) > board.getWidth() / 2) {
        dx += (dx > 0) ? -board.getWidth() : board.getWidth();
    }
    if (std::abs(dy) > board.getHeight() / 2) {
        dy += (dy > 0) ? -board.getHeight() : board.getHeight();
    }

    // Simple rule: find the most dominant direction
    if (std::abs(dx) >= std::abs(dy)) {
        if (dx > 0) return Direction::RIGHT;
        else        return Direction::LEFT;
    } else {
        if (dy > 0) return Direction::DOWN;
        else        return Direction::UP;
    }
}


std::string OffensiveAlgorithm::getRotationAction(const Tank& myTank, Direction targetDirection) const {
    Direction currentDir = myTank.getDirection();
    if (currentDir == targetDirection) {
        return "NONE";
    }

    // Try rotating right
    Direction temp = currentDir;
    for (int steps = 1; steps <= 4; ++steps) {
        temp = DirectionUtil::rotateRight(temp);
        if (temp == targetDirection) {
            return (steps == 1) ? "ROTATE_RIGHT" : "ROTATE_QUARTER_RIGHT";
        }
    }

    // Try rotating left
    temp = currentDir;
    for (int steps = 1; steps <= 4; ++steps) {
        temp = DirectionUtil::rotateLeft(temp);
        if (temp == targetDirection) {
            return (steps == 1) ? "ROTATE_LEFT" : "ROTATE_QUARTER_LEFT";
        }
    }

    return "NONE";
}


bool OffensiveAlgorithm::isAimedAtEnemyAfterRotation(const GameBoard& board, const Tank& myTank, const Tank& enemyTank, std::string rotation) {
    Tank rotatedTank = myTank;  // a copy
    
    Direction originalDir = myTank.getDirection();

    if(rotation == "ROTATE_LEFT") {
        rotatedTank.setDirection(DirectionUtil::rotateLeft(originalDir));
    } else if(rotation == "ROTATE_RIGHT") {
        rotatedTank.setDirection(DirectionUtil::rotateRight(originalDir));
    } else if(rotation == "ROTATE_QUARTER_LEFT") {
        rotatedTank.setDirection(DirectionUtil::rotateLeftQuarter(originalDir));
    } else if(rotation == "ROTATE_QUARTER_RIGHT") {
        rotatedTank.setDirection(DirectionUtil::rotateRightQuarter(originalDir));
    }

    return enemyInSight(board, myTank, enemyTank);
}


std::vector<std::pair<int, int>> OffensiveAlgorithm::bfsPath(const GameBoard& board, const std::pair<int, int>& start, const std::pair<int, int>& goal) {
    int width = board.getWidth();
    int height = board.getHeight();

    std::queue<std::pair<int, int>> my_queue;
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::vector<std::vector<std::pair<int, int>>> cameFrom(height, std::vector<std::pair<int, int>>(width, {-1, -1}));

    my_queue.push(start);
    visited[start.second][start.first] = true;

    // BFS loop
    while (!my_queue.empty()) {

        std::pair<int, int> current = my_queue.front();
        my_queue.pop();

        // Check if we reached the goal
        if (current == goal) {
            break;
        }

        //Loop over all possible movement directions
        for (Direction dir : DirectionUtil::allDirections()) {
            auto [dx, dy] = DirectionUtil::getMovement(dir);
            
            // Computing neighbor cell with wraparound (tunnel)
            int nx = (current.first + dx + width) % width;
            int ny = (current.second + dy + height) % height;

            //If the neighbor is unvisited and passable: Mark as visited, record that we came from current and add to queue for later exploration.
            if (!visited[ny][nx] && board.getObjectsAt(nx, ny).empty()) {
                visited[ny][nx] = true;
                cameFrom[ny][nx] = current;
                my_queue.push({nx, ny});
            }
        }
    }

    // No path found
    if (cameFrom[goal.second][goal.first] == std::pair<int, int>{-1, -1}) {
        return {};
    }

    // Reconstruct path by backtracking from goal using cameFrom.
    std::vector<std::pair<int, int>> path;
    std::pair<int, int> current = goal;
    while (current != start) {
        path.push_back(current);
        current = cameFrom[current.second][current.first];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}


std::string OffensiveAlgorithm::getChaseAction(const GameBoard& board, const Tank& myTank, const Tank& enemyTank) {
    static std::vector<std::pair<int, int>> cachedPath;
    static std::pair<int, int> cachedEnemyPos = {-1, -1};

    std::pair<int, int> myPos = myTank.getPosition();
    std::pair<int, int> enemyPos = enemyTank.getPosition();

    // If no cached path or enemy moved → recalculate
    if (cachedPath.empty() || cachedEnemyPos != enemyPos) {
        cachedPath = bfsPath(board, myPos, enemyPos);
        cachedEnemyPos = enemyPos;
    }

    // If no path found or path too short → no move
    if (cachedPath.size() < 2) {
        return "NONE";
    }

    std::pair<int, int> nextStep = cachedPath[1]; // Next position to move toward

    // Decide which direction to go
    int dx = (nextStep.first - myPos.first + board.getWidth()) % board.getWidth();
    int dy = (nextStep.second - myPos.second + board.getHeight()) % board.getHeight();

    // Fix toroidal wrap-around
    if (dx > board.getWidth() / 2) dx -= board.getWidth();
    if (dy > board.getHeight() / 2) dy -= board.getHeight();

    Direction desiredDir;
    if (dx == 1 && dy == 0) desiredDir = Direction::RIGHT;
    else if (dx == 1 && dy == 1) desiredDir = Direction::DOWN_RIGHT;
    else if (dx == 0 && dy == 1) desiredDir = Direction::DOWN;
    else if (dx == -1 && dy == 1) desiredDir = Direction::DOWN_LEFT;
    else if (dx == -1 && dy == 0) desiredDir = Direction::LEFT;
    else if (dx == -1 && dy == -1) desiredDir = Direction::UP_LEFT;
    else if (dx == 0 && dy == -1) desiredDir = Direction::UP;
    else if (dx == 1 && dy == -1) desiredDir = Direction::UP_RIGHT;
    else return "NONE"; // Should not happen

    // Rotate if needed
    std::string rotationAction = getRotationAction(myTank, desiredDir);
    if (rotationAction != "NONE") {
        return rotationAction;
    }

    // If already facing right → move forward
    return "MOVE_FORWARD";
}


std::string OffensiveAlgorithm::getNextAction(const GameBoard& board, int myTankId) {
    int enemyId = (myTankId == 1) ? 2 : 1;
    auto enemyTank = board.getPlayerTank(enemyId);
    auto myTank = board.getPlayerTank(myTankId);

    // 1. Check if need to evade (danger detection)
    if (isShellApproaching(board, *myTank)) {
        return getEvasionAction(board, *myTank);
    }

    // 2. If can shoot enemy, shoot
    if (enemyInSight(board, *myTank, *enemyTank) && canShootNow(*myTank)) {
        return "SHOOT";
    }

    // 3. Try to rotate to aim at the enemy
    Direction desiredDirection = getBestRotationToEnemy(board, *myTank, *enemyTank);
    if ((*myTank).getDirection() != desiredDirection) {
        
        std::string action = getRotationAction(*myTank, desiredDirection);
        if(isAimedAtEnemyAfterRotation(board, *myTank, *enemyTank, action)) {
            return action;
        }
    }

    // 4. Chase enemy using BFS path
    return getChaseAction(board, *myTank, *enemyTank);
}
