#include "OffensivePlayer.h"
#include "OffensiveTankAlgorithm.h"
#include <algorithm>
#include <limits>

OffensivePlayer::OffensivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : MyPlayer(player_index, x, y, max_steps, num_shells),
      aggression_level_(0.8) {  // High aggression by default
}

void OffensivePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    // 1. Find the tank's index
    int tank_index = findTankIndex(tank);
    
    // 2. Update board information
    updateBoardInfo(satellite_view, tank_index);
    
    // 3. Update target priorities
    updateTargetPriorities();
    
    // 4. Create attack plans
    createAttackPlans();
    
    // 5. Create offensive battle info
    OffensiveBattleInfo battle_info;
    
    // 6. Set basic tank info
    if (tank_index >= 0 && tank_index < static_cast<int>(my_tanks_.size())) {
        TankInfo& tank_info = my_tanks_[tank_index];
        battle_info.setCurrentPosition(tank_info.x, tank_info.y);
        battle_info.setCurrentDirection(tank_info.direction);
        battle_info.setRemainingShells(tank_info.remaining_shells);
        battle_info.setCooldownTimer(tank_info.cooldown_timer);
        battle_info.setBoardDimensions(board_width_, board_height_);
        battle_info.setGameState(current_step_, max_steps_);
        
        // 7. Check if tank is under threat
        int danger_level;
        std::vector<std::pair<size_t, size_t>> danger_sources;
        bool in_danger = isTankInDanger(tank_info, danger_level, danger_sources);
        battle_info.setUnderThreat(in_danger);
        
        for (const auto& source : danger_sources) {
            battle_info.addIncomingShell(source.first, source.second);
        }
        
        // 8. Set target information if we have an attack plan
        if (tank_index < static_cast<int>(attack_plans_.size())) {
            const AttackPlan& plan = attack_plans_[tank_index];
            
            if (plan.target_tank_index < enemy_tanks_.size()) {
                const TankInfo& target = enemy_tanks_[plan.target_tank_index];
                battle_info.setTarget(target.x, target.y, target.direction);
                
                // Add path to target
                for (const auto& point : plan.approach_path) {
                    battle_info.addPathPoint(point.first, point.second);
                }
                
                // Set shooting information
                battle_info.setCanHitTarget(plan.direct_fire_possible);
                battle_info.setShotsNeeded(1);  // Simplified for now
            }
        }
    }
    
    // 9. Update the tank's battle info
    tank.updateBattleInfo(battle_info);
}

void OffensivePlayer::updateTargetPriorities() {
    target_priorities_.clear();
    
    // For each enemy tank, calculate priority
    for (size_t i = 0; i < enemy_tanks_.size(); ++i) {
        const TankInfo& enemy = enemy_tanks_[i];
        
        // Base priority on distance to closest friendly tank
        double min_distance = std::numeric_limits<double>::max();
        for (const auto& my_tank : my_tanks_) {
            double distance = calculateDistance(my_tank.x, my_tank.y, enemy.x, enemy.y);
            min_distance = std::min(min_distance, distance);
        }
        
        // Higher priority for closer enemies
        double priority = 100.0 / (min_distance + 1.0);
        
        // Adjust priority based on line of sight
        for (const auto& my_tank : my_tanks_) {
            if (hasLineOfSight(my_tank.x, my_tank.y, enemy.x, enemy.y)) {
                priority *= 1.5;  // 50% bonus for direct line of sight
                break;
            }
        }
        
        target_priorities_.push_back({i, priority});
    }
    
    // Sort by priority (highest first)
    std::sort(target_priorities_.begin(), target_priorities_.end(),
             [](const TargetPriority& a, const TargetPriority& b) {
                 return a.priority > b.priority;
             });
}

void OffensivePlayer::createAttackPlans() {
    attack_plans_.clear();
    
    // Create a plan for each tank
    for (size_t i = 0; i < my_tanks_.size(); ++i) {
        // Skip if no enemies
        if (enemy_tanks_.empty() || target_priorities_.empty()) {
            attack_plans_.push_back(AttackPlan());
            continue;
        }
        
        // Assign targets based on priority
        size_t target_index = target_priorities_[i % target_priorities_.size()].tank_index;
        attack_plans_.push_back(createAttackPlan(i, target_index));
    }
}

OffensivePlayer::AttackPlan OffensivePlayer::createAttackPlan(size_t my_tank_index, size_t enemy_tank_index) {
    AttackPlan plan;
    plan.target_tank_index = enemy_tank_index;
    
    // Skip if invalid indices
    if (my_tank_index >= my_tanks_.size() || enemy_tank_index >= enemy_tanks_.size()) {
        return plan;
    }
    
    const TankInfo& my_tank = my_tanks_[my_tank_index];
    const TankInfo& enemy_tank = enemy_tanks_[enemy_tank_index];
    
    // Check if we can hit the target directly
    plan.direct_fire_possible = canHitTarget(my_tank.x, my_tank.y, my_tank.direction,
                                           enemy_tank.x, enemy_tank.y);
    
    // Calculate firing direction
    plan.firing_direction = calculateDirection(my_tank.x, my_tank.y, enemy_tank.x, enemy_tank.y);
    
    // Find path to target
    plan.approach_path = findPath(my_tank.x, my_tank.y, enemy_tank.x, enemy_tank.y);
    
    // Estimate steps needed
    plan.estimated_steps = plan.approach_path.size();
    
    return plan;
}

bool OffensivePlayer::canHitTarget(size_t from_x, size_t from_y, Direction direction, 
                                 size_t target_x, size_t target_y) const {
    // Check if target is in line with current direction
    Direction target_direction = calculateDirection(from_x, from_y, target_x, target_y);
    
    if (direction != target_direction) {
        return false;
    }
    
    // Check if there's a clear line of sight
    return hasLineOfSight(from_x, from_y, target_x, target_y);
}
