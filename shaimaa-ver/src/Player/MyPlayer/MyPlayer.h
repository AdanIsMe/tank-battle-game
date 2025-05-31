#pragma once
#include "../../common/Player.h"
#include "../../common/SatelliteView.h"
#include "../../common/TankAlgorithm.h"
#include "../../Game/Direction/Direction.h"
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <string>

// Base class for player implementations
class MyPlayer : public Player {
protected:
    // Player identification
    int player_index_;
    size_t initial_x_;
    size_t initial_y_;
    size_t max_steps_;
    size_t num_shells_;
    
    // Board information
    size_t board_width_;
    size_t board_height_;
    std::vector<std::vector<char>> last_board_state_;
    
    // Game step tracking
    size_t current_step_;
    
    // Tank tracking
    struct TankInfo {
        size_t x;
        size_t y;
        Direction direction;
        size_t remaining_shells;
        int cooldown_timer;
        bool is_alive;
        size_t last_info_step;  // Last step when this tank got info
        
        TankInfo(size_t x, size_t y, Direction direction, size_t shells)
            : x(x), y(y), direction(direction), remaining_shells(shells),
              cooldown_timer(0), is_alive(true), last_info_step(0) {}
        
        TankInfo(size_t x, size_t y, Direction direction, size_t shells,
           size_t cooldown_timer, bool is_alive, size_t last_info_step)
           : x(x), y(y), direction(direction), remaining_shells(shells),
           cooldown_timer(cooldown_timer), is_alive(is_alive), last_info_step(last_info_step) {}
    };
    
    // My tanks (indexed by tank_index)
    std::vector<TankInfo> my_tanks_;
    
    // Enemy tanks
    std::vector<TankInfo> enemy_tanks_;
    
    // Shell tracking
    struct ShellInfo {
        size_t x;
        size_t y;
        Direction direction;
        size_t first_seen_step;
        bool is_enemy;  // Is this an enemy shell?
        
        ShellInfo(size_t x, size_t y, Direction direction, size_t step, bool enemy)
            : x(x), y(y), direction(direction), first_seen_step(step), is_enemy(enemy) {}
    };
    
    std::vector<ShellInfo> observed_shells_;
    
    // Mine tracking
    std::vector<std::pair<size_t, size_t>> known_mines_;
    
    // Wall tracking
    struct WallInfo {
        size_t x;
        size_t y;
        int estimated_health;  // Estimated health (2 = new, 1 = damaged, 0 = destroyed)
        size_t last_seen_step;  // Last step when this wall was seen
        
        WallInfo(size_t x, size_t y, int health, size_t step)
            : x(x), y(y), estimated_health(health), last_seen_step(step) {}
        
        bool operator==(const WallInfo& other) const {
            return x == other.x && y == other.y;
        }
    };
    
    std::vector<WallInfo> known_walls_;
    
public:
    MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
    virtual ~MyPlayer() override = default;
    
    // This method must be implemented by derived classes
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override = 0;
    
protected:
    // Helper method to update tank and enemy information from satellite view
    void updateBoardInfo(SatelliteView& satellite_view, int tank_index);
    
    // Helper method to find a tank's index
    int findTankIndex(TankAlgorithm& tank) const;
    
    // Path finding utilities
    bool isValidPosition(size_t x, size_t y) const;
    bool isObstacle(size_t x, size_t y) const;
    std::vector<std::pair<size_t, size_t>> findPath(size_t start_x, size_t start_y, 
                                                   size_t goal_x, size_t goal_y) const;
    
    // Distance calculation
    double calculateDistance(size_t x1, size_t y1, size_t x2, size_t y2) const;// {
    //    return std::sqrt(std::pow(static_cast<double>(x2) - x1, 2) + 
    //                    std::pow(static_cast<double>(y2) - y1, 2));
    //}
    
    // Direction utilities
    Direction calculateDirection(size_t from_x, size_t from_y, size_t to_x, size_t to_y) const;
    std::vector<Direction> getRotationSequence(Direction current_dir, Direction target_dir) const;
    
    // Shell prediction
    std::vector<std::pair<size_t, size_t>> predictShellPath(size_t x, size_t y, 
                                                          Direction direction, int steps) const;
    
    // Danger assessment
    bool isTankInDanger(const TankInfo& tank, int& danger_level, 
                       std::vector<std::pair<size_t, size_t>>& danger_sources) const;
    
    // Safe position finding
    std::vector<std::pair<size_t, size_t>> findSafePositions(const TankInfo& tank) const;
    
    // Target acquisition
    std::pair<size_t, size_t> findClosestEnemyTank(size_t from_x, size_t from_y) const;
    
    // Line of sight calculation
    bool hasLineOfSight(size_t from_x, size_t from_y, size_t to_x, size_t to_y) const;
    
    // Update tank position
    void updateMyTankPosition(size_t x, size_t y, int excluding_tank_index);
    void updateEnemyTankPosition(size_t x, size_t y);
    
    // Update object positions
    void updateWallPosition(size_t x, size_t y);
    void updateMinePosition(size_t x, size_t y);
};
