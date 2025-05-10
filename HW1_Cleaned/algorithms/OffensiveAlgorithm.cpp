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
        std::string safe_move = findSafeDirection(board,x,y,dir,tank.getPlayerId()); // from tankAlgorithm
        if (safe_move != "NONE") {
            return safe_move;
        }
    }
    return "NONE"; // No evasion needed
}

// Check if enemy tank is in line of sight
bool OffensiveAlgorithm::enemyInSight(const GameBoard& board, const Tank& my_tank, const Tank& enemy_tank) {
    std::cout << "we're in the enemy in sight check function\n";
    std::pair<int, int> my_pos = my_tank.getPosition();
    std::pair<int, int> enemy_pos = enemy_tank.getPosition();
    auto [direction_x, direction_y] = DirectionUtil::getMovement(my_tank.getDirection());
    std::cout << "my_pos: (" << my_pos.first << ", " << my_pos.second << ")\n";
    std::cout << "enemy_pos: (" << enemy_pos.first << ", " << enemy_pos.second << ")\n";
    std::cout << "myDir: " << direction_x << " " << direction_y << "\n";
    // Check if enemy is generally in our facing direction
    if (!isInDirection(board, my_pos, enemy_pos, direction_x, direction_y)) {
        std::cout << "enemy is not in our facing direction\n";
        return false;
    }

    // Check each cell along the line for obstacles
    return hasClearPath(board, my_pos, enemy_pos);
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
    const Tank& my_tank,
    const Tank& enemy_tank)
{
    const std::pair<int, int> my_pos = my_tank.getPosition();
    const std::pair<int, int> enemy_pos = enemy_tank.getPosition();

    int dx = enemy_pos.first - my_pos.first;
    int dy = enemy_pos.second - my_pos.second;

    // Wrap-around adjustment (toroidal correction)
    if (std::abs(dx) > board.getWidth() / 2) {
        dx += (dx > 0) ? -board.getWidth() : board.getWidth();
    }
    if (std::abs(dy) > board.getHeight() / 2) {
        dy += (dy > 0) ? -board.getHeight() : board.getHeight();
    }

    Direction best_dir = DirectionUtil::getDirectionFromDelta(dx, dy);

    // Simulate rotating the tank and checking LoS
    Tank simulated_tank = my_tank;
    simulated_tank.setDirection(best_dir);

    bool has_line_of_sight = enemyInSight(board, simulated_tank, enemy_tank);

    return {has_line_of_sight, best_dir};
}

std::string OffensiveAlgorithm::getRotationAction(const Tank& my_tank, Direction target_direction) const {
    Direction current_dir = my_tank.getDirection();
    if (current_dir == target_direction) {
        return "NONE";
    }

    // Try rotating right
    Direction temp = current_dir;
    for (int steps = 1; steps <= 4; ++steps) {
        temp = DirectionUtil::rotateRight(temp);
        if (temp == target_direction) {
            if(steps == 1) return "ROTATE_RIGHT";
            if(steps == 2) return "ROTATE_RIGHT_QUARTER";
        }
    }

    // Try rotating left
    temp = current_dir;
    for (int steps = 1; steps <= 4; ++steps) {
        temp = DirectionUtil::rotateLeft(temp);
        if (temp == target_direction) {
            if(steps == 1) return "ROTATE_LEFT";
            if(steps == 2) return "ROTATE_LEFT_QUARTER";
        }
    }
    return "NONE";
}

bool OffensiveAlgorithm::isAimedAtEnemyAfterRotation(const GameBoard& board, const Tank& my_tank, const Tank& enemy_tank, std::string rotation) {
    Tank rotated_tank = my_tank;  // a copy
    
    Direction original_dir = my_tank.getDirection();

    if(rotation == "ROTATE_LEFT") {
        rotated_tank.setDirection(DirectionUtil::rotateLeft(original_dir));
    } else if(rotation == "ROTATE_RIGHT") {
        rotated_tank.setDirection(DirectionUtil::rotateRight(original_dir));
    } else if(rotation == "ROTATE_LEFT_QUARTER") {
        rotated_tank.setDirection(DirectionUtil::rotateLeftQuarter(original_dir));
    } else if(rotation == "ROTATE_RIGHT_QUARTER") {
        rotated_tank.setDirection(DirectionUtil::rotateRightQuarter(original_dir));
    }

    return enemyInSight(board, my_tank, enemy_tank);
}

std::vector<std::pair<int, int>> OffensiveAlgorithm::bfsPath(
    const GameBoard& board,
    const Tank& tank,
    const std::pair<int, int>& goal,
    int max_steps)
{
    const auto& start = tank.getPosition();
    Direction current_dir = tank.getDirection();

    int width = board.getWidth();
    int height = board.getHeight();

    // Queue stores {x, y, direction}
    using QueueEntry = std::tuple<int, int, Direction>;
    std::queue<QueueEntry> queue;
    
    // Visited tracks {x, y, direction} -> came from {x, y, direction}
    std::unordered_map<int, 
        std::unordered_map<int, 
            std::unordered_map<Direction, 
                std::tuple<int, int, Direction>>>> came_from;

    // Start BFS
    queue.push({start.first, start.second, current_dir});
    came_from[start.first][start.second][current_dir] = {-1, -1, current_dir}; // Sentinel for start

    int steps_taken = 0;
    bool goal_found = false;
    Direction goal_dir;

    // BFS loop
    while (!queue.empty() && (max_steps == -1 || steps_taken < max_steps)) {
        int level_size = queue.size();
        for (int i = 0; i < level_size; ++i) {
            auto [x, y, dir] = queue.front();
            queue.pop();

            // Check if goal reached
            if (x == goal.first && y == goal.second) {
                goal_found = true;
                goal_dir = dir;
                break;
            }

            // Try all valid rotations
            for (int rotation : {-2, -1, 0, 1, 2}) {
                Direction new_dir;
                if (rotation == 0) {
                    new_dir = dir;
                } else if (rotation == -1) {
                    new_dir = DirectionUtil::rotateLeft(dir);
                } else if (rotation == 1) {
                    new_dir = DirectionUtil::rotateRight(dir);
                } else if (rotation == -2) {
                    new_dir = DirectionUtil::rotateLeftQuarter(dir);
                } else if (rotation == 2) {
                    new_dir = DirectionUtil::rotateRightQuarter(dir);
                }

                auto [dx, dy] = DirectionUtil::getMovement(new_dir);
                int nx = (x + dx + width) % width;
                int ny = (y + dy + height) % height;

                // Skip if blocked or already visited with this direction
                if (!board.getObjectsAt(nx, ny).empty() || 
                    came_from[nx][ny].count(new_dir)) {
                    continue;
                }

                // Record path and enqueue
                came_from[nx][ny][new_dir] = {x, y, dir};
                queue.push({nx, ny, new_dir});
            }
        }
        if (goal_found) break;
        steps_taken++;
    }

    // Reconstruct path if goal was found
    if (!goal_found) return {};

    std::vector<std::pair<int, int>> path;
    auto current = goal;
    Direction dir = goal_dir;

    while (current.first != -1) {  // Until we reach the sentinel
        path.push_back(current);
        auto [px, py, pdir] = came_from[current.first][current.second][dir];
        current = {px, py};
        dir = pdir;
    }

    path.pop_back();  // Remove the sentinel position
    std::reverse(path.begin(), path.end());
    return path;
}

std::string OffensiveAlgorithm::getChaseAction(const GameBoard& board, const Tank& my_tank, const Tank& enemy_tank) {
    static std::vector<std::pair<int, int>> cached_path;
    static std::pair<int, int> cached_enemy_pos = {-1, -1};

    std::pair<int, int> my_pos = my_tank.getPosition();
    std::pair<int, int> enemy_pos = enemy_tank.getPosition();

    // If no cached path or enemy moved → recalculate
    if (cached_path.empty() || cached_enemy_pos != enemy_pos) {
        cached_path = bfsPath(board, my_tank, enemy_pos);
        cached_enemy_pos = enemy_pos;
    }

    // If no path found or path too short → no move
    if (cached_path.size() < 2) {
        auto [x, y] = my_tank.getPosition();
        Direction current_dir = my_tank.getDirection();
        auto [dx, dy] = DirectionUtil::getMovement(current_dir);
        int nx = x + dx;
        int ny = y + dy;
        board.wrapCoordinates(nx, ny);
    
        auto objects = board.getObjectsAt(nx, ny);
        if (isTileSafe(board, nx, ny)) {
            return "MOVE_FORWARD";
        }
    
        if (canShootNow(my_tank)) {
            for (const auto& obj : objects) {
                if (obj->isWall() || obj->isTank()) {
                    return "SHOOT";
                }
            }
        }
    
        // Try breaking out by rotating
        return "ROTATE_RIGHT";
    }

    std::pair<int, int> next_step = cached_path[1]; // Next position to move toward

    // Decide which direction to go
    int dx = (next_step.first - my_pos.first + board.getWidth()) % board.getWidth();
    int dy = (next_step.second - my_pos.second + board.getHeight()) % board.getHeight();

    // Fix toroidal wrap-around
    if (dx > board.getWidth() / 2) dx -= board.getWidth();
    if (dy > board.getHeight() / 2) dy -= board.getHeight();

    Direction desired_dir;
    if (dx == 1 && dy == 0) desired_dir = Direction::RIGHT;
    else if (dx == 1 && dy == 1) desired_dir = Direction::DOWN_RIGHT;
    else if (dx == 0 && dy == 1) desired_dir = Direction::DOWN;
    else if (dx == -1 && dy == 1) desired_dir = Direction::DOWN_LEFT;
    else if (dx == -1 && dy == 0) desired_dir = Direction::LEFT;
    else if (dx == -1 && dy == -1) desired_dir = Direction::UP_LEFT;
    else if (dx == 0 && dy == -1) desired_dir = Direction::UP;
    else if (dx == 1 && dy == -1) desired_dir = Direction::UP_RIGHT;
    else return "NONE"; // Should not happen

    // Rotate if needed
    std::string rotation_action = getRotationAction(my_tank, desired_dir);
    if (rotation_action != "NONE") {
        return rotation_action;
    }

    // If already facing right → move forward
    return "MOVE_FORWARD";
}

std::string OffensiveAlgorithm::getNextAction(const GameBoard& board, int my_tank_id) {
    int enemy_id = (my_tank_id == 1) ? 2 : 1;
    auto enemy_tank = board.getPlayerTank(enemy_id);
    auto my_tank = board.getPlayerTank(my_tank_id);

    // 1. Check if need to evade (danger detection)
    if (isShellApproaching(board, *my_tank)) {
        std::cout << "A SHELL IS APPROACHING TANK1, MUST DEFEND MYSELF\n";
        return getEvasionAction(board, *my_tank);
    }
    std::cout << "no shells are approaching tank1\n";

    // 2. If can shoot enemy, shoot
    if (canShootNow(*my_tank) && enemyInSight(board, *my_tank, *enemy_tank)) {
        std::cout << "TANK1 SHOOT\n";
        return "SHOOT";
    }

    // Move closer to enemy if in line of sight
    auto [direction_x, direction_y] = DirectionUtil::getMovement(my_tank->getDirection());
    if (isInDirection(board, (*my_tank).getPosition(), (*enemy_tank).getPosition(), direction_x, direction_y))
    {
        std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl;
        //check if the cell we would to go in this direction is the enemy tank, if so: NONE
        int next_x = (*my_tank).getPosition().first + direction_x;
        int next_y = (*my_tank).getPosition().second + direction_y;
        board.wrapCoordinates(next_x, next_y);
        std::cout << "???????????????????????????????????????????????\n";
        if(canShootNow(*my_tank)){ return "SHOOT";}
    
        auto objects = board.getObjectsAt(next_x,next_y);
        if(objects.size() == 0 ){
            return "MOVE_FORWARD";
        }
        if(canShootNow(*my_tank)){
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
    auto [can_see_enemy, desired_direction] = getBestRotationToEnemy(board, *my_tank, *enemy_tank);
    std::cout << "desired_direction: " << DirectionUtil::toString(desired_direction) << "\n";
    

    if ((*my_tank).getDirection() != desired_direction) {
        std::cout << "tank1 needs to rotate\n";
        std::string action = getRotationAction(*my_tank, desired_direction);
        std::cout << "action: " << action << "\n";
        if (can_see_enemy && action != "NONE") {
            std::cout << "Rotating to aim at enemy.\n";
            return action;
        }

    }
    
    // 4. Chase enemy using BFS path
    return getChaseAction(board, *my_tank, *enemy_tank);
}

std::string OffensiveAlgorithm::findSafeDirection(
    const GameBoard& board,
    int x, int y,
    Direction current_dir,
    int player_id
) const {
    auto tank = board.getPlayerTank(player_id);
    if (!tank) return "NONE";

    constexpr int lookahead_ticks = 3;
    bool immediate_danger = false;
    bool near_danger = false;

    // 1) Predict shell paths (shell moves 2 cells per tick)
    for (const auto& shell : board.getShells()) {
        auto [sx, sy] = shell->getPosition();
        Direction s_dir = shell->getDirection();
        auto [sdx, sdy] = DirectionUtil::getMovement(s_dir);

        for (int i = 1; i <= lookahead_ticks; ++i) {
            // step1 and step2 of shell in tick i
            int step1_x = (sx + sdx * (2*i - 1) + board.getWidth())  % board.getWidth();
            int step1_y = (sy + sdy * (2*i - 1) + board.getHeight()) % board.getHeight();
            int step2_x = (sx + sdx * (2*i    ) + board.getWidth())  % board.getWidth();
            int step2_y = (sy + sdy * (2*i    ) + board.getHeight()) % board.getHeight();

            if ((step1_x == x && step1_y == y) || (step2_x == x && step2_y == y)) {
                if (i == 1) immediate_danger = true;
                else         near_danger      = true;
                break;
            }
        }
        if (immediate_danger) break;  // worst case, no need to check other shells
    }

    // 2) Immediate danger: shell will land on you next tick
    if (immediate_danger) {
        // a) SHOOT if possible
        if (canShootNow(*tank) && canShootShell(board, x, y, current_dir)) {
            return "SHOOT";
        }
        // b) MOVE_FORWARD if safe
        {
            auto [dx, dy] = DirectionUtil::getMovement(current_dir);
            int fx = x + dx, fy = y + dy;
            board.wrapCoordinates(fx, fy);
            if (isTileSafe(board, fx, fy)) {
                return "MOVE_FORWARD";
            }
        }
        // c) Continue backward if already in that state
        if (tank->isInBackwardMove()) {
            auto [dx, dy] = DirectionUtil::getMovement(current_dir);
            int bx = x - dx, by = y - dy;
            board.wrapCoordinates(bx, by);
            if (isTileSafe(board, bx, by)) {
                return "MOVE_BACKWARD";
            }
        }
        // d) Try rotating into any safe adjacent tile
        for (Direction dir : DirectionUtil::allDirections()) {
            if (dir == current_dir) continue;
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
    if (near_danger) {
        for (Direction dir : DirectionUtil::allDirections()) {
            if (dir == current_dir) continue;
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
        auto [dx, dy] = DirectionUtil::getMovement(current_dir);
        int nx = x + dx, ny = y + dy;
        board.wrapCoordinates(nx, ny);
        if (isTileSafe(board, nx, ny)) {
            return "MOVE_FORWARD";
        }
    }

    // 5) Only move backward if already in the sequence
    if (tank->isInBackwardMove()) {
        auto [dx, dy] = DirectionUtil::getMovement(current_dir);
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