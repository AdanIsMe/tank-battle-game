#pragma once
#include "../MyBattleInfo/MyBattleInfo.h"
#include <vector>
#include <utility>

class OffensiveBattleInfo : public MyBattleInfo {
private:
    // Target information
    bool has_target_;
    size_t target_x_;
    size_t target_y_;
    Direction target_direction_;  // If known
    
    // Path to target (if using pathfinding)
    std::vector<std::pair<size_t, size_t>> path_to_target_;
    
    // Threat assessment
    bool under_threat_;
    std::vector<std::pair<size_t, size_t>> incoming_shells_;
    
    // Shooting opportunity assessment
    bool can_hit_target_;
    int shots_needed_;  // Estimated shots needed to hit target
    
public:
    OffensiveBattleInfo();
    ~OffensiveBattleInfo() override = default;
    
    // Target getters
    bool hasTarget() const { return has_target_; }
    size_t getTargetX() const { return target_x_; }
    size_t getTargetY() const { return target_y_; }
    Direction getTargetDirection() const { return target_direction_; }
    
    // Path getters
    const std::vector<std::pair<size_t, size_t>>& getPathToTarget() const {
        return path_to_target_;
    }
    
    // Threat getters
    bool isUnderThreat() const { return under_threat_; }
    const std::vector<std::pair<size_t, size_t>>& getIncomingShells() const {
        return incoming_shells_;
    }
    
    // Shooting opportunity getters
    bool canHitTarget() const { return can_hit_target_; }
    int getShotsNeeded() const { return shots_needed_; }
    
    // Target setters
    void setTarget(size_t x, size_t y, Direction direction = Direction::NONE);
    void clearTarget();
    
    // Path setters
    void addPathPoint(size_t x, size_t y);
    void clearPath();
    
    // Threat setters
    void setUnderThreat(bool under_threat);
    void addIncomingShell(size_t x, size_t y);
    void clearIncomingShells();
    
    // Shooting opportunity setters
    void setCanHitTarget(bool can_hit);
    void setShotsNeeded(int shots);
};
