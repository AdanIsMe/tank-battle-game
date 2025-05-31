#pragma once
#include "../MyPlayer/MyPlayer.h"
#include "../../BattleInfo/OffensiveBattleInfo/OffensiveBattleInfo.h"

class OffensivePlayer : public MyPlayer {
private:
    // Offensive strategy parameters
    double aggression_level_;  // 0.0-1.0, higher means more aggressive
    
    // Target priority queue (for multiple enemies)
    struct TargetPriority {
        size_t tank_index;  // Index in enemy_tanks_
        double priority;    // Higher means higher priority
        
        bool operator<(const TargetPriority& other) const {
            return priority < other.priority;
        }
    };
    
    std::vector<TargetPriority> target_priorities_;
    
    // Attack planning
    struct AttackPlan {
        size_t target_tank_index;
        std::vector<std::pair<size_t, size_t>> approach_path;
        bool direct_fire_possible;
        Direction firing_direction;
        size_t estimated_steps;
        
        AttackPlan() : target_tank_index(0), direct_fire_possible(false), 
                      firing_direction(Direction::NONE), estimated_steps(0) {}
    };
    
    std::vector<AttackPlan> attack_plans_;  // One per tank
    
public:
    OffensivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
    ~OffensivePlayer() override = default;
    
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
    
private:
    // Methods for offensive strategy
    void updateTargetPriorities();
    void createAttackPlans();
    AttackPlan createAttackPlan(size_t my_tank_index, size_t enemy_tank_index);
    bool canHitTarget(size_t from_x, size_t from_y, Direction direction, 
                     size_t target_x, size_t target_y) const;
};
