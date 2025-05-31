#include "GameManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <cmath>

GameManager::GameManager(const PlayerFactory& player_factory, const TankAlgorithmFactory& tank_algorithm_factory)
    : player_factory_(player_factory),
      tank_algorithm_factory_(tank_algorithm_factory),
      max_steps_(5000),
      num_shells_(16),
      rows_(0),
      cols_(0),
      current_step_(0) {
    
    // Initialize players and tanks vectors
    players_.resize(2);  // Two players
    tanks_.resize(2);    // Two players' tanks
}

GameManager::~GameManager() {
    if (output_stream_.is_open()) {
        output_stream_.close();
    }
}

void GameManager::readBoard(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }
    
    // Read header information
    std::string line;
    
    // Line 1: map name / description (ignore)
    std::getline(file, line);
    
    // Line 2: MaxSteps
    std::getline(file, line);
    if (line.find("MaxSteps") != std::string::npos) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string value = line.substr(pos + 1);
            max_steps_ = std::stoul(value);
        }
    }
    
    // Line 3: NumShells
    std::getline(file, line);
    if (line.find("NumShells") != std::string::npos) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string value = line.substr(pos + 1);
            num_shells_ = std::stoul(value);
        }
    }
    
    // Line 4: Rows
    std::getline(file, line);
    if (line.find("Rows") != std::string::npos) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string value = line.substr(pos + 1);
            rows_ = std::stoul(value);
        }
    }
    
    // Line 5: Cols
    std::getline(file, line);
    if (line.find("Cols") != std::string::npos) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string value = line.substr(pos + 1);
            cols_ = std::stoul(value);
        }
    }
    
    // Initialize board
    board_.resize(rows_, std::vector<char>(cols_, ' '));
    
    // Read the map
    size_t row = 0;
    std::map<int, std::vector<std::pair<size_t, size_t>>> player_tank_positions;
    
    while (std::getline(file, line) && row < rows_) {
        for (size_t col = 0; col < std::min(line.length(), cols_); ++col) {
            char c = line[col];
            board_[row][col] = c;
            
            // Process special characters
            if (c == '1' || c == '2') {
                int player_idx = c - '0';
                player_tank_positions[player_idx].push_back({col, row});
            }
        }
        row++;
    }
    
    // Initialize players and tanks
    for (const auto& [player_idx, positions] : player_tank_positions) {
        if (!positions.empty()) {
            // Use the first tank position for player initialization
            const auto& [x, y] = positions[0];
            players_[player_idx - 1] = player_factory_.create(player_idx, x, y, max_steps_, num_shells_);
            
            // Initialize tanks for this player
            for (size_t i = 0; i < positions.size(); ++i) {
                const auto& [tank_x, tank_y] = positions[i];
                tanks_[player_idx - 1].push_back(tank_algorithm_factory_.create(player_idx, i));
                
                // Add tank data
                Direction direction = (player_idx == 1) ? Direction::LEFT : Direction::RIGHT;
                tanks_data_.push_back(GameManager::TankData(player_idx, i, tank_x, tank_y, direction, num_shells_));
            }
        }
    }
    
    file.close();
}

void GameManager::runGame() {
    // Open output file
    output_stream_.open(output_file_);
    if (!output_stream_.is_open()) {
        std::cerr << "Error: Could not open output file " << output_file_ << std::endl;
        return;
    }
    
    // Initialize game
    updateBoard();
    
    // Main game loop
    while (current_step_ < max_steps_ && !isGameOver()) {
        processGameStep();
        current_step_++;
    }
    
    // Determine game result
    if (isGameOver()) {
        // Count alive tanks per player
        int player1_tanks = 0;
        int player2_tanks = 0;
        
        for (const auto& tank : tanks_data_) {
            if (tank.is_alive) {
                if (tank.player_index == 1) {
                    player1_tanks++;
                } else {
                    player2_tanks++;
                }
            }
        }
        
        if (player1_tanks > 0 && player2_tanks == 0) {
            logGameResult("Player 1 wins", "All Player 2 tanks destroyed");
        } else if (player1_tanks == 0 && player2_tanks > 0) {
            logGameResult("Player 2 wins", "All Player 1 tanks destroyed");
        } else {
            logGameResult("Tie", "Both players have tanks remaining after max steps");
        }
    } else {
        logGameResult("Tie", "Maximum steps reached");
    }
    
    // Close output file
    output_stream_.close();
}

void GameManager::writeOutput(const std::string& filename) {
    output_file_ = filename;
}

void GameManager::initializePlayers() {
    // Already initialized in readBoard
}

void GameManager::initializeTanks() {
    // Already initialized in readBoard
}

void GameManager::processGameStep() {
    // Process each player's tanks
    for (int player_idx = 1; player_idx <= 2; ++player_idx) {
        for (size_t tank_idx = 0; tank_idx < tanks_[player_idx-1].size(); ++tank_idx) {
            // Skip destroyed tanks
            bool tank_alive = false;
            for (const auto& tank_data : tanks_data_) {
                if (tank_data.player_index == player_idx && 
                    tank_data.tank_index == static_cast<int>(tank_idx) && 
                    tank_data.is_alive) {
                    tank_alive = true;
                    break;
                }
            }
            
            if (!tank_alive) {
                continue;
            }
            
            // Get the tank's action
            TankAlgorithm* tank = tanks_[player_idx-1][tank_idx].get();
            ActionRequest action = tank->getAction();
            
            // Process the action
            if (action == ActionRequest::GetBattleInfo) {
                // Find the tank data
                GameManager::TankData* tank_data = nullptr;
                for (auto& td : tanks_data_) {
                    if (td.player_index == player_idx && td.tank_index == static_cast<int>(tank_idx)) {
                        tank_data = &td;
                        break;
                    }
                }
                
                if (tank_data) {
                    // Create a satellite view for this tank
                    GameSatelliteView satellite_view(board_, tank_data->x, tank_data->y, player_idx);
                    
                    // Update the tank with battle info
                    players_[player_idx-1]->updateTankWithBattleInfo(*tank, satellite_view);
                }
                
                // Log the action
                logAction(player_idx, tank_idx, action, true);
            }
            else {
                // Process movement, shooting, or other actions
                processAction(player_idx, tank_idx, action);
            }
        }
    }
    
    // Move shells, check for collisions, etc.
    moveShells();
    checkCollisions();
    
    // Update the board representation
    updateBoard();
}

bool GameManager::isGameOver() {
    // Count alive tanks per player
    int player1_tanks = 0;
    int player2_tanks = 0;
    
    for (const auto& tank : tanks_data_) {
        if (tank.is_alive) {
            if (tank.player_index == 1) {
                player1_tanks++;
            } else {
                player2_tanks++;
            }
        }
    }
    
    // Game is over if either player has no tanks
    return player1_tanks == 0 || player2_tanks == 0;
}

void GameManager::moveShells() {
    for (auto& shell : shells_) {
        auto [dx, dy] = DirectionUtil::getMovement(shell.direction);
        shell.x = (shell.x + dx * 2 + cols_) % cols_;
        shell.y = (shell.y + dy * 2 + rows_) % rows_;
    }
}

void GameManager::checkCollisions() {
    // Check shell-wall collisions
    for (auto it = shells_.begin(); it != shells_.end(); ) {
        bool shell_destroyed = false;
        
        // Check if shell hit a wall
        if (board_[it->y][it->x] == '#') {
            // Damage the wall (in a real implementation, you'd track wall health)
            board_[it->y][it->x] = ' ';  // Simplified: wall is destroyed immediately
            shell_destroyed = true;
        }
        
        // Check if shell hit a tank
        for (auto& tank : tanks_data_) {
            if (tank.is_alive && tank.x == it->x && tank.y == it->y) {
                // Tank is hit
                tank.is_alive = false;
                shell_destroyed = true;
                break;
            }
        }
        
        // Remove shell if destroyed
        if (shell_destroyed) {
            it = shells_.erase(it);
        } else {
            ++it;
        }
    }
    
    // Check tank-mine collisions
    for (auto& tank : tanks_data_) {
        if (tank.is_alive && board_[tank.y][tank.x] == '@') {
            // Tank hit a mine
            tank.is_alive = false;
            board_[tank.y][tank.x] = ' ';  // Remove the mine
        }
    }
    
    // Check tank-tank collisions
    for (size_t i = 0; i < tanks_data_.size(); ++i) {
        for (size_t j = i + 1; j < tanks_data_.size(); ++j) {
            if (tanks_data_[i].is_alive && tanks_data_[j].is_alive &&
                tanks_data_[i].x == tanks_data_[j].x && tanks_data_[i].y == tanks_data_[j].y) {
                // Tanks collided
                tanks_data_[i].is_alive = false;
                tanks_data_[j].is_alive = false;
            }
        }
    }
}

void GameManager::updateBoard() {
    // Clear the board
    for (auto& row : board_) {
        std::fill(row.begin(), row.end(), ' ');
    }
    
    // Place walls and mines (in a real implementation, you'd track these separately)
    // For simplicity, we're not updating walls and mines here
    
    // Place tanks
    for (const auto& tank : tanks_data_) {
        if (tank.is_alive) {
            board_[tank.y][tank.x] = '0' + tank.player_index;
        }
    }
    
    // Place shells
    for (const auto& shell : shells_) {
        board_[shell.y][shell.x] = '*';
    }
}

void GameManager::processAction(int player_idx, int tank_idx, ActionRequest action) {
    // Find the tank data
    GameManager::TankData* tank_data = nullptr;
    for (auto& td : tanks_data_) {
        if (td.player_index == player_idx && td.tank_index == tank_idx) {
            tank_data = &td;
            break;
        }
    }

    if (!tank_data || !tank_data->is_alive) {
        logAction(player_idx, tank_idx, action, false);
        return;
    }

    bool is_valid = true;

    switch (action) {
        case ActionRequest::MoveForward: {
            auto [dx, dy] = DirectionUtil::getMovement(tank_data->direction);
            size_t new_x = (tank_data->x + dx + cols_) % cols_;
            size_t new_y = (tank_data->y + dy + rows_) % rows_;

            if (board_[new_y][new_x] != '#') {
                tank_data->x = new_x;
                tank_data->y = new_y;
            } else {
                is_valid = false;
            }
            break;
        }

        case ActionRequest::MoveBackward: {
            auto [dx, dy] = DirectionUtil::getMovement(tank_data->direction);
            size_t new_x = (tank_data->x - dx + cols_) % cols_;
            size_t new_y = (tank_data->y - dy + rows_) % rows_;

            if (board_[new_y][new_x] != '#') {
                tank_data->x = new_x;
                tank_data->y = new_y;
            } else {
                is_valid = false;
            }
            break;
        }

        case ActionRequest::RotateLeft45: {
            tank_data->direction = DirectionUtil::rotateLeft(tank_data->direction);
            break;
        }

        case ActionRequest::RotateRight45: {
            tank_data->direction = DirectionUtil::rotateRight(tank_data->direction);
            break;
        }

        case ActionRequest::RotateLeft90: {
            tank_data->direction = DirectionUtil::rotateLeftQuarter(tank_data->direction);
            break;
        }

        case ActionRequest::RotateRight90: {
            tank_data->direction = DirectionUtil::rotateRightQuarter(tank_data->direction);
            break;
        }

        default:
            is_valid = false;
            break;
    }

    logAction(player_idx, tank_idx, action, is_valid);
}


void GameManager::logAction(int player_idx, int tank_idx, ActionRequest action, bool is_valid) {
    if (!output_stream_.is_open()) {
        return;
    }
    
    // Convert action to string
    std::string action_str;
    switch (action) {
        case ActionRequest::MoveForward: action_str = "Move Forward"; break;
        case ActionRequest::MoveBackward: action_str = "Move Backward"; break;
        case ActionRequest::RotateLeft45: action_str = "Rotate Left 45"; break;
        case ActionRequest::RotateRight45: action_str = "Rotate Right 45"; break;
        case ActionRequest::RotateLeft90: action_str = "Rotate Left 90"; break;
        case ActionRequest::RotateRight90: action_str = "Rotate Right 90"; break;
        case ActionRequest::Shoot: action_str = "Shoot"; break;
        case ActionRequest::GetBattleInfo: action_str = "Get Battle Info"; break;
        case ActionRequest::DoNothing: action_str = "Do Nothing"; break;
        default: action_str = "Unknown Action"; break;
    }
    
    // Log the action
    output_stream_ << "Step " << current_step_ << ": Player " << player_idx 
                  << ", Tank " << tank_idx << " - " << action_str;
    
    if (!is_valid) {
        output_stream_ << " (Invalid)";
    }
    
    output_stream_ << std::endl;
}

void GameManager::logGameResult(const std::string& result, const std::string& reason) {
    if (!output_stream_.is_open()) {
        return;
    }
    
    output_stream_ << "\nGame Result: " << result << "\n";
    output_stream_ << "Reason: " << reason << "\n";
    output_stream_ << "Total Steps: " << current_step_ << "\n";
}

//////// From HW1: Utility function to get movement deltas based on direction ////////

std::pair<int, int> GameManager::DirectionUtil::getMovement(Direction dir) {
    switch (dir) {
        case GameManager::Direction::UP: return {0, -1};
        case GameManager::Direction::UP_RIGHT: return {1, -1};
        case GameManager::Direction::RIGHT: return {1, 0};
        case GameManager::Direction::DOWN_RIGHT: return {1, 1};
        case GameManager::Direction::DOWN: return {0, 1};
        case GameManager::Direction::DOWN_LEFT: return {-1, 1};
        case GameManager::Direction::LEFT: return {-1, 0};
        case GameManager::Direction::UP_LEFT: return {-1, -1};
        default: return {0, 0};
    }
}

GameManager::Direction GameManager::DirectionUtil::rotateLeft(Direction dir) {
    switch (dir) {
        case Direction::UP: return Direction::UP_LEFT;
        case Direction::UP_RIGHT: return Direction::UP;
        case Direction::RIGHT: return Direction::UP_RIGHT;
        case Direction::DOWN_RIGHT: return Direction::RIGHT;
        case Direction::DOWN: return Direction::DOWN_RIGHT;
        case Direction::DOWN_LEFT: return Direction::DOWN;
        case Direction::LEFT: return Direction::DOWN_LEFT;
        case Direction::UP_LEFT: return Direction::LEFT;
        default: return dir;
    }
}

GameManager::Direction GameManager::DirectionUtil::rotateRight(Direction dir) {
    switch (dir) {
        case Direction::UP: return Direction::UP_RIGHT;
        case Direction::UP_RIGHT: return Direction::RIGHT;
        case Direction::RIGHT: return Direction::DOWN_RIGHT;
        case Direction::DOWN_RIGHT: return Direction::DOWN;
        case Direction::DOWN: return Direction::DOWN_LEFT;
        case Direction::DOWN_LEFT: return Direction::LEFT;
        case Direction::LEFT: return Direction::UP_LEFT;
        case Direction::UP_LEFT: return Direction::UP;
        default: return dir;
    }
}

GameManager::Direction GameManager::DirectionUtil::rotateLeftQuarter(Direction dir) {
    return rotateLeft(rotateLeft(dir));
}

GameManager::Direction GameManager::DirectionUtil::rotateRightQuarter(Direction dir) {
    return rotateRight(rotateRight(dir));
}

std::string GameManager::DirectionUtil::toString(Direction dir) {
    switch (dir) {
        case Direction::UP: return "U";
        case Direction::UP_RIGHT: return "UR";
        case Direction::RIGHT: return "R";
        case Direction::DOWN_RIGHT: return "DR";
        case Direction::DOWN: return "D";
        case Direction::DOWN_LEFT: return "DL";
        case Direction::LEFT: return "L";
        case Direction::UP_LEFT: return "UL";
        default: return "";
    }
}

GameManager::Direction GameManager::DirectionUtil::fromString(const std::string& str) {
    if (str == "U") return Direction::UP;
    if (str == "UR") return Direction::UP_RIGHT;
    if (str == "R") return Direction::RIGHT;
    if (str == "DR") return Direction::DOWN_RIGHT;
    if (str == "D") return Direction::DOWN;
    if (str == "DL") return Direction::DOWN_LEFT;
    if (str == "L") return Direction::LEFT;
    if (str == "UL") return Direction::UP_LEFT;
    return Direction::UP; // default
}

GameManager::Direction GameManager::DirectionUtil::getDirectionFromDelta(int dx, int dy) {
    if (dx == 0 && dy < 0) return Direction::UP;
    if (dx > 0 && dy < 0) return Direction::UP_RIGHT;
    if (dx > 0 && dy == 0) return Direction::RIGHT;
    if (dx > 0 && dy > 0) return Direction::DOWN_RIGHT;
    if (dx == 0 && dy > 0) return Direction::DOWN;
    if (dx < 0 && dy > 0) return Direction::DOWN_LEFT;
    if (dx < 0 && dy == 0) return Direction::LEFT;
    if (dx < 0 && dy < 0) return Direction::UP_LEFT;
    return Direction::UP; // Default/fallback
}

std::string GameManager::DirectionUtil::getActionToMove(Direction currentDir, int dx, int dy) {
    Direction targetDir = getDirectionFromDelta(dx, dy);

    if (currentDir == targetDir) {
        return "MOVE_FORWARD";
    }

    // Determine shortest rotation
    if (rotateLeft(currentDir) == targetDir) {
        return "ROTATE_LEFT";
    } else if (rotateRight(currentDir) == targetDir) {
        return "ROTATE_RIGHT";
    } else {
        // Not just one rotation away, so rotate left as default
        return "ROTATE_LEFT";
    }
}

std::vector<GameManager::Direction> GameManager::DirectionUtil::allDirections() {
    return {
        Direction::UP,
        Direction::LEFT,
        Direction::RIGHT,
        Direction::DOWN_LEFT,
        Direction::UP_LEFT,
        Direction::UP_RIGHT,
        Direction::DOWN_RIGHT,
        Direction::DOWN
    };
}

