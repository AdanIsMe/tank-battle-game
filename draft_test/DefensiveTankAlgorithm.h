#pragma once
#include "MyTankAlgorithm.h"
#include "DefensiveBattleInfo.h"

class DefensiveTankAlgorithm : public MyTankAlgorithm {
private:
    // Danger assessment (copied from battle info)
    bool in_danger_;
    int danger_level_;
    std::vector<std::pair<size_t, size_t>> danger_sources_;
    
    // Escape route if in danger
    bool has_escape_route_;
    std::vector<std::pair<size_t, size_t>> escape_route_;
    
    // Tanks to defend if not in danger
    std::vector<std::pair<size_t, size_t>> tanks_to_defend_;
    
    // Safe positions
    std::vector<std::pair<size_t, size_t>> safe_positions_;
    
    // Action state tracking
    int action_counter_;
    
public:
    DefensiveTankAlgorithm(int player_index, int tank_index);
    ~DefensiveTankAlgorithm() override = default;
    
    ActionRequest getAction() override;
    
protected:
    void updateSpecificBattleInfo(BattleInfo& info) override;
    
private:
    ActionRequest determineDefensiveAction();
    ActionRequest escapeFromDanger();
    ActionRequest defendAllies();
    ActionRequest moveToSafePosition();
    ActionRequest attackIfPossible();
};
