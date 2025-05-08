#include "../algorithms/OffensiveAlgorithm.h"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <iostream> // For print statements

std::string OffensiveAlgorithm::getEvasionAction(const GameBoard& board, const Tank& tank) const
{

    auto [x, y] = tank.getPosition();
    Direction dir = tank.getDirection();
    
    // Check if we're in danger (shell coming towards us)
    if (isShellApproaching(board, tank)) {
        std::string safeMove = findSafeDirection(board,x,y,dir,tank.getPlayerId()); // from tankAlgorithm
        if (safeMove != "NONE") {
            return safeMove;
        }
    }
    return "NONE"; // No evasion needed
}

// Check if enemy tank is in line of sight
bool OffensiveAlgorithm::enemyInSight(const GameBoard& board, const Tank& myTank, const Tank& enemyTank) {
    std::cout << "we're in the enemy in sight check function\n";
    std::pair<int, int> myPos = myTank.getPosition();
    std::pair<int, int> enemyPos = enemyTank.getPosition();
    auto [direction_x, direction_y] = DirectionUtil::getMovement(myTank.getDirection());
    std::cout << "myPos: (" << myPos.first << ", " << myPos.second << ")\n";
    std::cout << "enemyPos: (" << enemyPos.first << ", " << enemyPos.second << ")\n";
    std::cout << "myDir: " << direction_x << " " << direction_y << "\n";
    // Check if enemy is generally in our facing direction
    if (!isInDirection(board, myPos, enemyPos, direction_x, direction_y)) {
        std::cout << "enemy is not in our facing direction\n";
        return false;
    }

    // Check each cell along the line for obstacles
    return hasClearPath(board, myPos, enemyPos);
}

bool OffensiveAlgorithm::hasClearPath(const GameBoard& board, std::pair<int, int> start, std::pair<int, int> end) {
    std::cout << "we're in the hasClearShot function\n";
    int x0 = start.first;
    int y0 = start.second;
    int x1 = end.first;
    int y1 = end.second;

    // If start and end are the same, path is clear
    if (x0 == x1 && y0 == y1) {
        return true;
    }

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int board_width = board.getWidth();
    int board_height = board.getHeight();

    while (true) {
        // Check current cell (except starting position)
        if (!(x0 == start.first && y0 == start.second)) {
            // Handle board wrapping
            int wrapped_x = (x0 + board_width) % board_width;
            int wrapped_y = (y0 + board_height) % board_height;

            if (board.cellHasWall(wrapped_x, wrapped_y)) {
                return false;
            }
        }

        // If we've reached the end point
        if (x0 == x1 && y0 == y1) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }

    return true;
}

// Checks if the shoot cooldown elapsed
bool OffensiveAlgorithm::canShootNow(const Tank& tank)const {
    std::cout << "___________can i shoot?????????????" << std::endl;

    return (tank.getShootCooldown() == 0 && tank.getShellsLeft() > 0);
}

std::pair<bool, Direction> OffensiveAlgorithm::getBestRotationToEnemy(
    const GameBoard& board,
    const Tank& myTank,
    const Tank& enemyTank)
{
    const std::pair<int, int> myPos = myTank.getPosition();
    const std::pair<int, int> enemyPos = enemyTank.getPosition();

    int dx = enemyPos.first - myPos.first;
    int dy = enemyPos.second - myPos.second;

    // Wrap-around adjustment (toroidal correction)
    if (std::abs(dx) > board.getWidth() / 2) {
        dx += (dx > 0) ? -board.getWidth() : board.getWidth();
    }
    if (std::abs(dy) > board.getHeight() / 2) {
        dy += (dy > 0) ? -board.getHeight() : board.getHeight();
    }

    Direction bestDir = DirectionUtil::getDirectionFromDelta(dx, dy);

    // Simulate rotating the tank and checking LoS
    Tank simulatedTank = myTank;
    simulatedTank.setDirection(bestDir);

    bool hasLineOfSight = enemyInSight(board, simulatedTank, enemyTank);

    return {hasLineOfSight, bestDir};
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
            if(steps == 1) return "ROTATE_RIGHT";
            if(steps == 2) return "ROTATE_RIGHT_QUARTER";
        }
    }

    // Try rotating left
    temp = currentDir;
    for (int steps = 1; steps <= 4; ++steps) {
        temp = DirectionUtil::rotateLeft(temp);
        if (temp == targetDirection) {
            if(steps == 1) return "ROTATE_LEFT";
            if(steps == 2) return "ROTATE_LEFT_QUARTER";
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
    } else if(rotation == "ROTATE_LEFT_QUARTER") {
        rotatedTank.setDirection(DirectionUtil::rotateLeftQuarter(originalDir));
    } else if(rotation == "ROTATE_RIGHT_QUARTER") {
        rotatedTank.setDirection(DirectionUtil::rotateRightQuarter(originalDir));
    }

    return enemyInSight(board, myTank, enemyTank);
}

std::vector<std::pair<int, int>> OffensiveAlgorithm::bfsPath(
    const GameBoard& board,
    const Tank& tank,
    const std::pair<int, int>& goal,
    int maxSteps)
{
    const auto& start = tank.getPosition();
    Direction currentDir = tank.getDirection();

    int width = board.getWidth();
    int height = board.getHeight();

    // Queue stores {x, y, direction}
    using QueueEntry = std::tuple<int, int, Direction>;
    std::queue<QueueEntry> queue;
    
    // Visited tracks {x, y, direction} -> came from {x, y, direction}
    std::unordered_map<int, 
        std::unordered_map<int, 
            std::unordered_map<Direction, 
                std::tuple<int, int, Direction>>>> cameFrom;

    // Start BFS
    queue.push({start.first, start.second, currentDir});
    cameFrom[start.first][start.second][currentDir] = {-1, -1, currentDir}; // Sentinel for start

    int stepsTaken = 0;
    bool goalFound = false;
    Direction goalDir;

    // BFS loop
    while (!queue.empty() && (maxSteps == -1 || stepsTaken < maxSteps)) {
        int levelSize = queue.size();
        for (int i = 0; i < levelSize; ++i) {
            auto [x, y, dir] = queue.front();
            queue.pop();

            // Check if goal reached
            if (x == goal.first && y == goal.second) {
                goalFound = true;
                goalDir = dir;
                break;
            }

            // Try all valid rotations
            for (int rotation : {-2, -1, 0, 1, 2}) {
                Direction newDir;
                if (rotation == 0) {
                    newDir = dir;
                } else if (rotation == -1) {
                    newDir = DirectionUtil::rotateLeft(dir);
                } else if (rotation == 1) {
                    newDir = DirectionUtil::rotateRight(dir);
                } else if (rotation == -2) {
                    newDir = DirectionUtil::rotateLeftQuarter(dir);
                } else if (rotation == 2) {
                    newDir = DirectionUtil::rotateRightQuarter(dir);
                }

                auto [dx, dy] = DirectionUtil::getMovement(newDir);
                int nx = (x + dx + width) % width;
                int ny = (y + dy + height) % height;

                // Skip if blocked or already visited with this direction
                if (!board.getObjectsAt(nx, ny).empty() || 
                    cameFrom[nx][ny].count(newDir)) {
                    continue;
                }

                // Record path and enqueue
                cameFrom[nx][ny][newDir] = {x, y, dir};
                queue.push({nx, ny, newDir});
            }
        }
        if (goalFound) break;
        stepsTaken++;
    }

    // Reconstruct path if goal was found
    if (!goalFound) return {};

    std::vector<std::pair<int, int>> path;
    auto current = goal;
    Direction dir = goalDir;

    while (current.first != -1) {  // Until we reach the sentinel
        path.push_back(current);
        auto [px, py, pdir] = cameFrom[current.first][current.second][dir];
        current = {px, py};
        dir = pdir;
    }

    path.pop_back();  // Remove the sentinel position
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
        cachedPath = bfsPath(board, myTank, enemyPos);
        cachedEnemyPos = enemyPos;
    }

    // If no path found or path too short → no move
    if (cachedPath.size() < 2) {
        auto [x, y] = myTank.getPosition();
        Direction currentDir = myTank.getDirection();
        auto [dx, dy] = DirectionUtil::getMovement(currentDir);
        int nx = x + dx;
        int ny = y + dy;
        board.wrapCoordinates(nx, ny);
    
        auto objects = board.getObjectsAt(nx, ny);
        if (isTileSafe(board, nx, ny)) {
            return "MOVE_FORWARD";
        }
    
        if (canShootNow(myTank)) {
            for (const auto& obj : objects) {
                if (obj->isWall() || obj->isTank()) {
                    return "SHOOT";
                }
            }
        }
    
        // Try breaking out by rotating
        return "ROTATE_RIGHT";
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
        std::cout << "A SHELL IS APPROACHING TANK1, MUST DEFEND MYSELF\n";
        return getEvasionAction(board, *myTank);
    }
    std::cout << "no shells are approaching tank1\n";

    // 2. If can shoot enemy, shoot
    if (canShootNow(*myTank) && enemyInSight(board, *myTank, *enemyTank)) {
        std::cout << "TANK1 SHOOT\n";
        return "SHOOT";
    }

    // Move closer to enemy if in line of sight
    auto [direction_x, direction_y] = DirectionUtil::getMovement(myTank->getDirection());
    if (isInDirection(board, (*myTank).getPosition(), (*enemyTank).getPosition(), direction_x, direction_y))
    {
        std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl;
        //check if the cell we would to go in this direction is the enemy tank, if so: NONE
        int nextX = (*myTank).getPosition().first + direction_x;
        int nextY = (*myTank).getPosition().second + direction_y;
        board.wrapCoordinates(nextX, nextY);
        std::cout << "???????????????????????????????????????????????\n";
        if(canShootNow(*myTank)){ return "SHOOT";}
    
        auto objects = board.getObjectsAt(nextX,nextY);
        if(objects.size() == 0 ){
            return "MOVE_FORWARD";
        }
        if(canShootNow(*myTank)){
            for (const auto& obj : objects) {
                //if (obj == shell) continue;
                if (obj->isWall()) {
                        return "SHOOT";
                    }
                }
        }
        //return "MOVE_FORWARD";
    }

    // 3. Try to rotate to aim at the enemy
    auto [canSeeEnemy, desiredDirection] = getBestRotationToEnemy(board, *myTank, *enemyTank);
    std::cout << "desiredDirection: " << DirectionUtil::toString(desiredDirection) << "\n";
    

    if ((*myTank).getDirection() != desiredDirection) {
        std::cout << "tank1 needs to rotate\n";
        std::string action = getRotationAction(*myTank, desiredDirection);
        std::cout << "action: " << action << "\n";
        if (canSeeEnemy && action!= "NONE") {
            std::cout << "Rotating to aim at enemy.\n";
            return action;
        }

    }
    
    // 4. Chase enemy using BFS path
    return getChaseAction(board, *myTank, *enemyTank);
}

std::string OffensiveAlgorithm::findSafeDirection(
    const GameBoard& board,
    int x, int y,
    Direction currentDir,
    int playerId
) const {
    auto tank = board.getPlayerTank(playerId);
    if (!tank) return "NONE";

    constexpr int LOOKAHEAD_TICKS = 3;
    bool immediateDanger = false;
    bool nearDanger      = false;

    // 1) Predict shell paths (shell moves 2 cells per tick)
    for (const auto& shell : board.getShells()) {
        auto [sx, sy] = shell->getPosition();
        Direction sdir = shell->getDirection();
        auto [sdx, sdy] = DirectionUtil::getMovement(sdir);

        for (int i = 1; i <= LOOKAHEAD_TICKS; ++i) {
            // step1 and step2 of shell in tick i
            int step1x = (sx + sdx * (2*i - 1) + board.getWidth())  % board.getWidth();
            int step1y = (sy + sdy * (2*i - 1) + board.getHeight()) % board.getHeight();
            int step2x = (sx + sdx * (2*i    ) + board.getWidth())  % board.getWidth();
            int step2y = (sy + sdy * (2*i    ) + board.getHeight()) % board.getHeight();

            if ((step1x == x && step1y == y) || (step2x == x && step2y == y)) {
                if (i == 1) immediateDanger = true;
                else         nearDanger      = true;
                break;
            }
        }
        if (immediateDanger) break;  // worst case, no need to check other shells
    }

    // 2) Immediate danger: shell will land on you next tick
    if (immediateDanger) {
        // a) SHOOT if possible
        if (canShootNow(*tank) && canShootShell(board, x, y, currentDir)) {
            return "SHOOT";
        }
        // b) MOVE_FORWARD if safe
        {
            auto [dx, dy] = DirectionUtil::getMovement(currentDir);
            int fx = x + dx, fy = y + dy;
            board.wrapCoordinates(fx, fy);
            if (isTileSafe(board, fx, fy)) {
                return "MOVE_FORWARD";
            }
        }
        // c) Continue backward if already in that state
        if (tank->isInBackwardMove()) {
            auto [dx, dy] = DirectionUtil::getMovement(currentDir);
            int bx = x - dx, by = y - dy;
            board.wrapCoordinates(bx, by);
            if (isTileSafe(board, bx, by)) {
                return "MOVE_BACKWARD";
            }
        }
        // d) Try rotating into any safe adjacent tile
        for (Direction dir : DirectionUtil::allDirections()) {
            if (dir == currentDir) continue;
            auto [dx, dy] = DirectionUtil::getMovement(dir);
            int tx = x + dx, ty = y + dy;
            board.wrapCoordinates(tx, ty);
            if (isTileSafe(board, tx, ty)) {
                std::string rotate = getRotationAction(*tank, dir);
                if (rotate != "NONE") return rotate;
            }
        }
        // e) Last-ditch: rotate right
        return "ROTATE_RIGHT";
    }

    // 3) Near danger: shell will hit you soon (not next tick)
    if (nearDanger) {
        for (Direction dir : DirectionUtil::allDirections()) {
            if (dir == currentDir) continue;
            auto [dx, dy] = DirectionUtil::getMovement(dir);
            int tx = x + dx, ty = y + dy;
            board.wrapCoordinates(tx, ty);
            if (isTileSafe(board, tx, ty)) {
                std::string rotate = getRotationAction(*tank, dir);
                if (rotate != "NONE") return rotate;
            }
        }
    }

    // 4) No immediate or near danger: proceed normally
    {
        auto [dx, dy] = DirectionUtil::getMovement(currentDir);
        int nx = x + dx, ny = y + dy;
        board.wrapCoordinates(nx, ny);
        if (isTileSafe(board, nx, ny)) {
            return "MOVE_FORWARD";
        }
    }

    // 5) Only move backward if already in the sequence
    if (tank->isInBackwardMove()) {
        auto [dx, dy] = DirectionUtil::getMovement(currentDir);
        int bx = x - dx, by = y - dy;
        board.wrapCoordinates(bx, by);
        if (isTileSafe(board, bx, by)) {
            return "MOVE_BACKWARD";
        }
    }

    // 6) Otherwise, rotate to stay alive
    return "ROTATE_RIGHT";
}

bool OffensiveAlgorithm::isTileSafe(const GameBoard& board, int tx, int ty) const{
    board.wrapCoordinates(tx, ty);
    
    // Check for collisions with walls or mines
    for (const auto& obj : board.getObjectsAt(tx, ty)) {
        if (!obj->isShell()) {
            return false;
        }
    }
    
    // Check for incoming shells
    for (const auto& shell : board.getShells()) {
        auto [sx, sy] = shell->getPosition();
        auto [sdx, sdy] = DirectionUtil::getMovement(shell->getDirection());
        
        // Check current and next shell positions
        if ((sx + sdx == tx && sy+ sdx  == ty) || 
            (sx + 2*sdx == tx && sy + 2*sdy == ty)) {
            return false;
        }
    }
    
    return true;
}

bool OffensiveAlgorithm::canShootShell(const GameBoard& board, int x, int y, Direction dir) const{
    for (const auto& shell : board.getShells()) {
        //if (shell->getPlayerId() == playerId) continue;
        
        auto [sx, sy] = shell->getPosition();
        if (dir == Direction::LEFT && sy == y && sx < x) return true;
        if (dir == Direction::RIGHT && sy == y && sx > x) return true;
        if (dir == Direction::UP && sx == x && sy < y) return true;
        if (dir == Direction::DOWN && sx == x && sy > y) return true;
    }
    return false;
}