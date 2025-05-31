#pragma once
#include"../MyTankAlgorithm/MyTankAlgorithm.h"
#include "../../BattleInfo/OffensiveBattleInfo/OffensiveBattleInfo.h"

class OffensiveTankAlgorithm : public MyTankAlgorithm {
private:
    // Target coordinates (copied from battle info)
    bool has_target_;
    size_t target_x_;
    size_t target_y_;
    
    // Path to target (if using pathfinding)
    std::vector<std::pair<size_t, size_t>> path_to_target_;
    
    // Threat assessment
    bool under_threat_;
    std::vector<std::pair<size_t, size_t>> incoming_shells_;
    
    // Shooting opportunity assessment
    bool can_hit_target_;
    int shots_needed_;
    
    // Action state tracking
    int action_counter_;
    
public:
    OffensiveTankAlgorithm(int player_index, int tank_index);
    ~OffensiveTankAlgorithm() override = default;
    
    ActionRequest getAction() override;
    
protected:
    void updateSpecificBattleInfo(BattleInfo& info) override;
    
private:
    ActionRequest determineOffensiveAction();
    ActionRequest handleThreat();
    ActionRequest attackTarget();
    ActionRequest followPathToTarget();
};
