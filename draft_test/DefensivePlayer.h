#pragma once
#include "MyPlayer.h"
#include "DefensiveBattleInfo.h"

class DefensivePlayer : public MyPlayer {
private:
    // Defensive strategy parameters
    double caution_level_;  // 0.0-1.0, higher means more cautious
    size_t danger_zone_radius_;  // How far away is considered "danger zone"
    
    // Defensive zone tracking
    struct DefensiveZone {
        size_t center_x;
        size_t center_y;
        size_t radius;
        std::vector<size_t> tanks_in_zone;  // Indices of my tanks in this zone
        
        DefensiveZone(size_t x, size_t y, size_t r) 
            : center_x(x), center_y(y), radius(r) {}
    };
    
    std::vector<DefensiveZone> defensive_zones_;
    
    // Tank role assignment
    enum class TankRole { Defender, Runner, Attacker };
    std::vector<TankRole> tank_roles_;
    
    // Defense planning
    struct DefensePlan {
        size_t tank_index;
        size_t tank_to_defend;  // Index of tank to defend, if applicable
        std::vector<std::pair<size_t, size_t>> movement_path;
        bool should_shoot;
        size_t target_x;
        size_t target_y;
        
        DefensePlan() : tank_index(0), tank_to_defend(0), should_shoot(false),
                       target_x(0), target_y(0) {}
    };
    
    std::vector<DefensePlan> defense_plans_;  // One per tank
    
public:
    DefensivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
    ~DefensivePlayer() override = default;
    
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
    
private:
    // Methods for defensive strategy
    void updateDefensiveZones();
    void assignTankRoles();
    void createDefensePlans();
    DefensePlan createDefensePlan(size_t tank_index);
    std::vector<std::pair<size_t, size_t>> findEscapeRoute(const TankInfo& tank);
    bool isInDangerZone(size_t x, size_t y) const;
};
