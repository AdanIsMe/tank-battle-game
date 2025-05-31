#include "DefensivePlayer.h"
#include "DefensiveTankAlgorithm.h"
#include <algorithm>
#include <limits>
#include <cmath>

DefensivePlayer::DefensivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : MyPlayer(player_index, x, y, max_steps, num_shells),
      caution_level_(0.7),  // High caution by default
      danger_zone_radius_(5) {  // 5 squares is danger zone
    
    // Initialize tank roles
    tank_roles_.push_back(TankRole::Defender);
}

void DefensivePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    // 1. Find the tank's index
    int tank_index = findTankIndex(tank);
    
    // 2. Update board information
    updateBoardInfo(satellite_view, tank_index);
    
    // 3. Update defensive zones
    updateDefensiveZones();
    
    // 4. Assign tank roles
    assignTankRoles();
    
    // 5. Create defense plans
    createDefensePlans();
    
    // 6. Create defensive battle info
    DefensiveBattleInfo battle_info;
    
    // 7. Set basic tank info
    if (tank_index >= 0 && tank_index < static_cast<int>(my_tanks_.size())) {
        TankInfo& tank_info = my_tanks_[tank_index];
        battle_info.setCurrentPosition(tank_info.x, tank_info.y);
        battle_info.setCurrentDirection(tank_info.direction);
        battle_info.setRemainingShells(tank_info.remaining_shells);
        battle_info.setCooldownTimer(tank_info.cooldown_timer);
        battle_info.setBoardDimensions(board_width_, board_height_);
        battle_info.setGameState(current_step_, max_steps_);
        
        // 8. Check if tank is in danger
        int danger_level;
        std::vector<std::pair<size_t, size_t>> danger_sources;
        bool in_danger = isTankInDanger(tank_info, danger_level, danger_sources);
        battle_info.setDanger(in_danger, danger_level);
        
        for (const auto& source : danger_sources) {
            battle_info.addDangerSource(source.first, source.second);
        }
        
        // 9. Set escape route if in danger
        if (in_danger) {
            std::vector<std::pair<size_t, size_t>> escape_route = findEscapeRoute(tank_info);
            battle_info.setHasEscapeRoute(!escape_route.empty());
            
            for (const auto& point : escape_route) {
                battle_info.addEscapePoint(point.first, point.second);
            }
        }
        
        // 10. Set tanks to defend if not in danger
        if (!in_danger && tank_index < static_cast<int>(defense_plans_.size())) {
            const DefensePlan& plan = defense_plans_[tank_index];
            
            if (plan.tank_to_defend < my_tanks_.size()) {
                const TankInfo& tank_to_defend = my_tanks_[plan.tank_to_defend];
                battle_info.addTankToDefend(tank_to_defend.x, tank_to_defend.y);
            }
            
            // Add movement path
            for (const auto& point : plan.movement_path) {
                battle_info.addSafePosition(point.first, point.second);
            }
        }
    }
    
    // 11. Update the tank's battle info
    tank.updateBattleInfo(battle_info);
}

void DefensivePlayer::updateDefensiveZones() {
    defensive_zones_.clear();
    
    // Create a defensive zone around each tank
    for (size_t i = 0; i < my_tanks_.size(); ++i) {
        const TankInfo& tank = my_tanks_[i];
        
        // Create a zone centered on the tank
        DefensiveZone zone(tank.x, tank.y, danger_zone_radius_);
        zone.tanks_in_zone.push_back(i);
        
        // Add other tanks that are in this zone
        for (size_t j = 0; j < my_tanks_.size(); ++j) {
            if (i != j) {
                const TankInfo& other_tank = my_tanks_[j];
                double distance = calculateDistance(tank.x, tank.y, other_tank.x, other_tank.y);
                
                if (distance <= danger_zone_radius_) {
                    zone.tanks_in_zone.push_back(j);
                }
            }
        }
        
        defensive_zones_.push_back(zone);
    }
    
    // Merge overlapping zones
    for (size_t i = 0; i < defensive_zones_.size(); ++i) {
        for (size_t j = i + 1; j < defensive_zones_.size(); /* no increment */) {
            DefensiveZone& zone1 = defensive_zones_[i];
            DefensiveZone& zone2 = defensive_zones_[j];
            
            double distance = calculateDistance(zone1.center_x, zone1.center_y, 
                                              zone2.center_x, zone2.center_y);
            
            if (distance <= zone1.radius + zone2.radius) {
                // Merge zones
                for (size_t tank_idx : zone2.tanks_in_zone) {
                    if (std::find(zone1.tanks_in_zone.begin(), zone1.tanks_in_zone.end(), tank_idx) 
                        == zone1.tanks_in_zone.end()) {
                        zone1.tanks_in_zone.push_back(tank_idx);
                    }
                }
                
                // Remove zone2
                defensive_zones_.erase(defensive_zones_.begin() + j);
            } else {
                ++j;
            }
        }
    }
}

void DefensivePlayer::assignTankRoles() {
    // Resize roles vector if needed
    if (tank_roles_.size() != my_tanks_.size()) {
        tank_roles_.resize(my_tanks_.size(), TankRole::Defender);
    }
    
    // Analyze game state
    for (size_t i = 0; i < my_tanks_.size(); ++i) {
        if (my_tanks_[i].is_alive) {
            int danger_level;
            std::vector<std::pair<size_t, size_t>> danger_sources;
            bool in_danger = isTankInDanger(my_tanks_[i], danger_level, danger_sources);
            
            if (in_danger) {
                tank_roles_[i] = TankRole::Runner;
            } else {
                // Check if any other tank needs defending
                bool needs_to_defend = false;
                for (size_t j = 0; j < my_tanks_.size(); ++j) {
                    if (i != j && tank_roles_[j] == TankRole::Runner) {
                        tank_roles_[i] = TankRole::Defender;
                        needs_to_defend = true;
                        break;
                    }
                }
                
                if (!needs_to_defend) {
                    tank_roles_[i] = TankRole::Attacker;
                }
            }
        }
    }
}

void DefensivePlayer::createDefensePlans() {
    defense_plans_.clear();
    
    // Create a plan for each tank
    for (size_t i = 0; i < my_tanks_.size(); ++i) {
        defense_plans_.push_back(createDefensePlan(i));
    }
}

DefensivePlayer::DefensePlan DefensivePlayer::createDefensePlan(size_t tank_index) {
    DefensePlan plan;
    plan.tank_index = tank_index;
    
    // Skip if invalid index
    if (tank_index >= my_tanks_.size()) {
        return plan;
    }
    
    const TankInfo& tank = my_tanks_[tank_index];
    
    // Handle based on role
    switch (tank_roles_[tank_index]) {
        case TankRole::Runner: {
            // Find escape route
            plan.movement_path = findEscapeRoute(tank);
            plan.should_shoot = false;  // Focus on escaping
            break;
        }
        
        case TankRole::Defender: {
            // Find a tank to defend
            for (size_t i = 0; i < my_tanks_.size(); ++i) {
                if (i != tank_index && tank_roles_[i] == TankRole::Runner) {
                    plan.tank_to_defend = i;
                    
                    // Find path to the tank to defend
                    const TankInfo& tank_to_defend = my_tanks_[i];
                    plan.movement_path = findPath(tank.x, tank.y, tank_to_defend.x, tank_to_defend.y);
                    
                    // Check if we should shoot at any enemy
                    for (const auto& enemy : enemy_tanks_) {
                        if (hasLineOfSight(tank.x, tank.y, enemy.x, enemy.y)) {
                            plan.should_shoot = true;
                            plan.target_x = enemy.x;
                            plan.target_y = enemy.y;
                            break;
                        }
                    }
                    
                    break;
                }
            }
            break;
        }
        
        case TankRole::Attacker: {
            // Find closest enemy
            auto closest = findClosestEnemyTank(tank.x, tank.y);
            
            // Find path to the enemy
            plan.movement_path = findPath(tank.x, tank.y, closest.first, closest.second);
            
            // Check if we should shoot
            if (hasLineOfSight(tank.x, tank.y, closest.first, closest.second)) {
                plan.should_shoot = true;
                plan.target_x = closest.first;
                plan.target_y = closest.second;
            }
            break;
        }
    }
    
    return plan;
}

std::vector<std::pair<size_t, size_t>> DefensivePlayer::findEscapeRoute(const TankInfo& tank) {
    // Find safe positions
    auto safe_positions = findSafePositions(tank);
    
    // If no safe positions, return empty path
    if (safe_positions.empty()) {
        return {};
    }
    
    // Find the safest position (furthest from danger)
    std::pair<size_t, size_t> safest_pos = safe_positions[0];
    double max_safety = 0;
    
    for (const auto& pos : safe_positions) {
        double min_distance = std::numeric_limits<double>::max();
        
        // Check distance to enemy tanks
        for (const auto& enemy : enemy_tanks_) {
            double distance = calculateDistance(pos.first, pos.second, enemy.x, enemy.y);
            min_distance = std::min(min_distance, distance);
        }
        
        // Check distance to observed shells
        for (const auto& shell : observed_shells_) {
            double distance = calculateDistance(pos.first, pos.second, shell.x, shell.y);
            min_distance = std::min(min_distance, distance);
        }
        
        if (min_distance > max_safety) {
            max_safety = min_distance;
            safest_pos = pos;
        }
    }
    
    // Find path to safest position
    return findPath(tank.x, tank.y, safest_pos.first, safest_pos.second);
}

bool DefensivePlayer::isInDangerZone(size_t x, size_t y) const {
    // Check if position is in range of enemy tanks
    for (const auto& enemy : enemy_tanks_) {
        double distance = calculateDistance(x, y, enemy.x, enemy.y);
        
        // If within danger radius, it's a danger zone
        if (distance <= danger_zone_radius_) {
            return true;
        }
        
        // If enemy has line of sight, it's a danger zone
        if (hasLineOfSight(enemy.x, enemy.y, x, y)) {
            return true;
        }
    }
    
    // Check if position is in path of enemy shells
    for (const auto& shell : observed_shells_) {
        auto future_positions = predictShellPath(shell.x, shell.y, shell.direction, 5);
        
        for (const auto& pos : future_positions) {
            if (pos.first == x && pos.second == y) {
                return true;
            }
        }
    }
    
    return false;
}
