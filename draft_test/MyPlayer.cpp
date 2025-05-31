#include "MyPlayer.h"
#include <limits>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <sstream>
#include <unordered_map>

MyPlayer::MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : Player(player_index, x, y, max_steps, num_shells),
      player_index_(player_index),
      initial_x_(x),
      initial_y_(y),
      max_steps_(max_steps),
      num_shells_(num_shells),
      board_width_(100),  // Default values, will be updated from satellite view
      board_height_(100),
      current_step_(0) {
    
    // Initialize board state with default size
    last_board_state_ = std::vector<std::vector<char>>(
        board_height_, std::vector<char>(board_width_, ' '));
    
    // Initialize with the first tank
    my_tanks_.push_back(TankInfo(x, y,
    (player_index == 1) ? Direction::LEFT : Direction::RIGHT,
    num_shells));

}

void MyPlayer::updateBoardInfo(SatelliteView& satellite_view, int tank_index) {
    // First scan to determine board dimensions
    size_t max_x = 0;
    size_t max_y = 0;
    
    for (size_t y = 0; y < 1000; ++y) {
        bool found_boundary = false;
        for (size_t x = 0; x < 1000; ++x) {
            char object = satellite_view.getObjectAt(x, y);
            if (object == '&') {
                found_boundary = true;
                break;
            }
            max_x = std::max(max_x, x);
        }
        if (found_boundary && y > 0) {
            max_y = y - 1;
            break;
        }
    }
    
    // Update board dimensions if needed
    if (max_x > 0 && max_y > 0) {
        board_width_ = max_x + 1;
        board_height_ = max_y + 1;
        
        // Resize board state if needed
        if (last_board_state_.size() != board_height_ || 
            (last_board_state_.size() > 0 && last_board_state_[0].size() != board_width_)) {
            last_board_state_ = std::vector<std::vector<char>>(
                board_height_, std::vector<char>(board_width_, ' '));
        }
    }
    
    // Clear previous observations
    observed_shells_.clear();
    
    // Find the requesting tank's position
    size_t tank_x = 0, tank_y = 0;
    bool found_tank = false;
    
    // Scan the entire board
    for (size_t y = 0; y < board_height_; ++y) {
        for (size_t x = 0; x < board_width_; ++x) {
            char object = satellite_view.getObjectAt(x, y);
            
            // Update last_board_state_
            last_board_state_[y][x] = object;
            
            // Process specific objects
            if (object == '%') {
                // This is the requesting tank
                tank_x = x;
                tank_y = y;
                found_tank = true;
                
                // Update the tank's position in my_tanks_
                if (tank_index >= 0 && tank_index < static_cast<int>(my_tanks_.size())) {
                    my_tanks_[tank_index].x = x;
                    my_tanks_[tank_index].y = y;
                }
            }
            else if (object == std::to_string(player_index_)[0]) {
                // This is one of my tanks (but not the requesting one)
                updateMyTankPosition(x, y, tank_index);
            }
            else if (object == std::to_string(3 - player_index_)[0]) {
                // This is an enemy tank
                updateEnemyTankPosition(x, y);
            }
            else if (object == '*') {
                // This is a shell
                observed_shells_.push_back(ShellInfo(x, y, Direction::NONE, current_step_, false));
            }
            else if (object == '#') {
                // This is a wall
                updateWallPosition(x, y);
            }
            else if (object == '@') {
                // This is a mine
                updateMinePosition(x, y);
            }
        }
    }
    
    // If we found the requesting tank, update its position
    if (found_tank && tank_index >= 0 && tank_index < static_cast<int>(my_tanks_.size())) {
        my_tanks_[tank_index].x = tank_x;
        my_tanks_[tank_index].y = tank_y;
        my_tanks_[tank_index].last_info_step = current_step_;
    }
    
    // Increment current step
    current_step_++;
}

int MyPlayer::findTankIndex(TankAlgorithm& tank) const {
    // In a real implementation, you would need a way to map TankAlgorithm objects to indices
    // This is a simplified version that assumes the tank index is available
    // For example, you might store a map of TankAlgorithm* to indices
    
    // For now, return 0 as we're starting with just one tank
    return 0;
}

bool MyPlayer::isValidPosition(size_t x, size_t y) const {
    return x < board_width_ && y < board_height_;
}

bool MyPlayer::isObstacle(size_t x, size_t y) const {
    if (!isValidPosition(x, y)) {
        return true;
    }
    
    char object = last_board_state_[y][x];
    return object == '#' || object == '@';
}

std::vector<std::pair<size_t, size_t>> MyPlayer::findPath(
    size_t start_x, size_t start_y, size_t goal_x, size_t goal_y) const {
    // A* pathfinding algorithm
    
    // Define a node for A*
    struct Node {
        size_t x, y;
        double g_cost;  // Cost from start
        double h_cost;  // Heuristic cost to goal
        double f_cost;  // g_cost + h_cost
        std::pair<size_t, size_t> parent;
        
        Node(size_t x, size_t y, double g, double h, std::pair<size_t, size_t> parent)
            : x(x), y(y), g_cost(g), h_cost(h), f_cost(g + h), parent(parent) {}
        
        bool operator>(const Node& other) const {
            return f_cost > other.f_cost;
        }
    };
    
    // Priority queue for open set
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set;
    
    // Set of visited nodes
    std::unordered_set<size_t> closed_set;
    
    // Hash function for node positions
    auto hash_position = [this](size_t x, size_t y) {
        return y * board_width_ + x;
    };
    
    // Heuristic function (Manhattan distance)
    auto heuristic = [](size_t x, size_t y, size_t goal_x, size_t goal_y) {
        return std::abs(static_cast<int>(goal_x) - static_cast<int>(x)) + 
               std::abs(static_cast<int>(goal_y) - static_cast<int>(y));
    };
    
    // Start with the initial position
    open_set.push(Node(start_x, start_y, 0, heuristic(start_x, start_y, goal_x, goal_y), 
                      {start_x, start_y}));
    
    // Directions: up, right, down, left, and diagonals
    const std::vector<std::pair<int, int>> directions = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0},
        {1, -1}, {1, 1}, {-1, 1}, {-1, -1}
    };
    
    // Map to store parent nodes for reconstructing the path
    std::unordered_map<size_t, std::pair<size_t, size_t>> came_from;
    
    while (!open_set.empty()) {
        // Get the node with the lowest f_cost
        Node current = open_set.top();
        open_set.pop();
        
        // Check if we've reached the goal
        if (current.x == goal_x && current.y == goal_y) {
            // Reconstruct the path
            std::vector<std::pair<size_t, size_t>> path;
            std::pair<size_t, size_t> current_pos = {current.x, current.y};
            
            while (current_pos.first != start_x || current_pos.second != start_y) {
                path.push_back(current_pos);
                current_pos = came_from[hash_position(current_pos.first, current_pos.second)];
            }
            
            // Reverse the path to get start-to-goal order
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Add current node to closed set
        closed_set.insert(hash_position(current.x, current.y));
        
        // Check all neighbors
        for (const auto& dir : directions) {
            size_t new_x = current.x + dir.first;
            size_t new_y = current.y + dir.second;
            
            // Skip if out of bounds or is an obstacle
            if (isObstacle(new_x, new_y)) {
                continue;
            }
            
            // Skip if already in closed set
            if (closed_set.find(hash_position(new_x, new_y)) != closed_set.end()) {
                continue;
            }
            
            // Calculate g_cost for this neighbor
            double new_g_cost = current.g_cost + 1;  // Assuming all moves cost 1
            
            // Create neighbor node
            Node neighbor(new_x, new_y, new_g_cost, 
                         heuristic(new_x, new_y, goal_x, goal_y), 
                         {current.x, current.y});
            
            // Add to open set
            open_set.push(neighbor);
            
            // Store parent for path reconstruction
            came_from[hash_position(new_x, new_y)] = {current.x, current.y};
        }
    }
    
    // No path found
    return {};
}

Direction MyPlayer::calculateDirection(size_t from_x, size_t from_y, size_t to_x, size_t to_y) const {
    int dx = static_cast<int>(to_x) - static_cast<int>(from_x);
    int dy = static_cast<int>(to_y) - static_cast<int>(from_y);

    return DirectionUtil::getDirectionFromDelta(dx, dy);
}

std::vector<Direction> MyPlayer::getRotationSequence(Direction current_dir, Direction target_dir) const {
    std::vector<Direction> rotations;
    Direction temp_dir = current_dir;
    
    // Calculate the shortest rotation path (clockwise or counter-clockwise)
    // Map Direction to angle (in 45-degree increments)
    static const std::unordered_map<Direction, int> dir_to_angle = {
        {Direction::UP, 0},
        {Direction::UP_RIGHT, 1},
        {Direction::RIGHT, 2},
        {Direction::DOWN_RIGHT, 3},
        {Direction::DOWN, 4},
        {Direction::DOWN_LEFT, 5},
        {Direction::LEFT, 6},
        {Direction::UP_LEFT, 7}
    };

    int current_angle = dir_to_angle.at(current_dir);
    int target_angle = dir_to_angle.at(target_dir);

    int diff = (target_angle - current_angle + 8) % 8;
    int clockwise_steps = diff;
    int counter_clockwise_steps = (8 - diff) % 8;

    // Choose the shortest rotation path
    if (clockwise_steps <= counter_clockwise_steps) {
        // Rotate clockwise
        while (temp_dir != target_dir) {
            temp_dir = DirectionUtil::rotateRight(temp_dir);
            rotations.push_back(temp_dir);
        }
    } else {
        // Rotate counter-clockwise
        while (temp_dir != target_dir) {
            temp_dir = DirectionUtil::rotateLeft(temp_dir);
            rotations.push_back(temp_dir);
        }
    }

    return rotations;
}

std::vector<std::pair<size_t, size_t>> MyPlayer::predictShellPath(
    size_t x, size_t y, Direction direction, int steps) const {
    std::vector<std::pair<size_t, size_t>> path;
    
    // Get movement vector for the direction
    auto movement = DirectionUtil::getMovement(direction);
    int dx = movement.first * 2;  // Shells move 2 squares at a time
    int dy = movement.second * 2;
    
    // Predict path for given steps
    for (int i = 1; i <= steps; ++i) {
        x = (x + dx + board_width_) % board_width_;
        y = (y + dy + board_height_) % board_height_;
        
        path.push_back({x, y});
        
        // Stop if we hit a wall
        if (isObstacle(x, y)) {
            break;
        }
    }
    
    return path;
}

bool MyPlayer::isTankInDanger(const TankInfo& tank, int& danger_level, 
                             std::vector<std::pair<size_t, size_t>>& danger_sources) const {
    danger_level = 0;
    danger_sources.clear();
    
    // Check for nearby enemy tanks
    for (const auto& enemy : enemy_tanks_) {
        double distance = calculateDistance(tank.x, tank.y, enemy.x, enemy.y);
        
        // If within danger radius, it's a danger
        if (distance <= 3) {
            danger_level = std::max(danger_level, 5);
            danger_sources.push_back({enemy.x, enemy.y});
        }
        
        // If enemy has line of sight, it's a danger
        if (hasLineOfSight(enemy.x, enemy.y, tank.x, tank.y)) {
            danger_level = std::max(danger_level, 7);
            danger_sources.push_back({enemy.x, enemy.y});
        }
    }
    
    // Check for shells
    for (const auto& shell : observed_shells_) {
        // Predict shell path
        auto future_positions = predictShellPath(shell.x, shell.y, shell.direction, 5);
        
        for (const auto& pos : future_positions) {
            if (pos.first == tank.x && pos.second == tank.y) {
                danger_level = 10;  // Maximum danger
                danger_sources.push_back({shell.x, shell.y});
                break;
            }
        }
    }
    
    // Check for mines
    for (const auto& mine : known_mines_) {
        if (mine.first == tank.x && mine.second == tank.y) {
            danger_level = 10;  // Maximum danger
            danger_sources.push_back(mine);
        }
    }
    
    return danger_level > 0;
}

std::vector<std::pair<size_t, size_t>> MyPlayer::findSafePositions(const TankInfo& tank) const {
    std::vector<std::pair<size_t, size_t>> safe_positions;
    
    // Check all adjacent positions
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            // Skip current position
            if (dx == 0 && dy == 0) {
                continue;
            }
            
            size_t new_x = (tank.x + dx + board_width_) % board_width_;
            size_t new_y = (tank.y + dy + board_height_) % board_height_;
            
            // Skip obstacles
            if (isObstacle(new_x, new_y)) {
                continue;
            }
            
            // Check if position is safe
            int danger_level;
            std::vector<std::pair<size_t, size_t>> danger_sources;
            bool in_danger = isTankInDanger({new_x, new_y, tank.direction, tank.remaining_shells, 
                                           tank.cooldown_timer, true, tank.last_info_step}, 
                                          danger_level, danger_sources);
            
            if (!in_danger) {
                safe_positions.push_back({new_x, new_y});
            }
        }
    }
    
    return safe_positions;
}

std::pair<size_t, size_t> MyPlayer::findClosestEnemyTank(size_t from_x, size_t from_y) const {
    double min_distance = std::numeric_limits<double>::max();
    std::pair<size_t, size_t> closest = {0, 0};
    bool found = false;
    
    for (const auto& enemy : enemy_tanks_) {
        double distance = MyPlayer::calculateDistance(from_x, from_y, enemy.x, enemy.y);
        
        if (distance < min_distance) {
            min_distance = distance;
            closest = {enemy.x, enemy.y};
            found = true;
        }
    }
    
    return found ? closest : std::make_pair(from_x, from_y);  // Return own position if no enemies
}

bool MyPlayer::hasLineOfSight(size_t from_x, size_t from_y, size_t to_x, size_t to_y) const {
    // Bresenham's line algorithm
    int dx = std::abs(static_cast<int>(to_x) - static_cast<int>(from_x));
    int dy = std::abs(static_cast<int>(to_y) - static_cast<int>(from_y));
    int sx = from_x < to_x ? 1 : -1;
    int sy = from_y < to_y ? 1 : -1;
    int err = dx - dy;
    
    while (from_x != to_x || from_y != to_y) {
        // Check if current position is an obstacle
        if (isObstacle(from_x, from_y)) {
            return false;
        }
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            from_x += sx;
        }
        if (e2 < dx) {
            err += dx;
            from_y += sy;
        }
    }
    
    return true;
}

void MyPlayer::updateMyTankPosition(size_t x, size_t y, int excluding_tank_index) {
    // Check if this position matches any of our known tanks
    for (size_t i = 0; i < my_tanks_.size(); ++i) {
        if (static_cast<int>(i) != excluding_tank_index) {
            if (my_tanks_[i].x == x && my_tanks_[i].y == y) {
                // Update last info step
                my_tanks_[i].last_info_step = current_step_;
                return;
            }
        }
    }
    
    // If we get here, this is a new tank
    // In a real implementation, you'd need to determine the direction and other properties
    Direction default_direction = (player_index_ == 1) ? Direction::LEFT : Direction::RIGHT;
    my_tanks_.push_back(TankInfo(x, y, default_direction, num_shells_));
}

void MyPlayer::updateEnemyTankPosition(size_t x, size_t y) {
    // Check if this position matches any of our known enemy tanks
    for (auto& enemy : enemy_tanks_) {
        if (enemy.x == x && enemy.y == y) {
            // Update last info step
            enemy.last_info_step = current_step_;
            return;
        }
    }
    
    // If we get here, this is a new enemy tank
    // In a real implementation, you'd need to determine the direction and other properties
    Direction default_direction = (player_index_ == 1) ? Direction::RIGHT : Direction::LEFT;
    enemy_tanks_.push_back(TankInfo(x, y, default_direction, num_shells_));
}

void MyPlayer::updateWallPosition(size_t x, size_t y) {
    // Check if already known
    auto it = std::find_if(known_walls_.begin(), known_walls_.end(),
        [x, y](const WallInfo& wall) {
            return wall.x == x && wall.y == y;
        });

    if (it == known_walls_.end()) {
        known_walls_.push_back(WallInfo(x, y, /*estimated_health=*/2, current_step_));
    } else {
        // Optional: Update last seen step
        it->last_seen_step = current_step_;
    }
}




void MyPlayer::updateMinePosition(size_t x, size_t y) {
    // Add to known mines if not already there
    auto it = std::find(known_mines_.begin(), known_mines_.end(), std::make_pair(x, y));
    if (it == known_mines_.end()) {
        known_mines_.push_back({x, y});
    }
}

double MyPlayer::calculateDistance(size_t x1, size_t y1, size_t x2, size_t y2) const {
    int dx = std::abs(static_cast<int>(x2) - static_cast<int>(x1));
    int dy = std::abs(static_cast<int>(y2) - static_cast<int>(y1));
    
    // Account for board wrapping
    dx = std::min(dx, static_cast<int>(board_width_) - dx);
    dy = std::min(dy, static_cast<int>(board_height_) - dy);
    
    return std::sqrt(dx * dx + dy * dy);
}
