#pragma once
#include "../../common/TankAlgorithm.h"
#include "../../Game/Direction/Direction.h"
#include "../../BattleInfo/MyBattleInfo/MyBattleInfo.h"

// Base class for tank algorithm implementations
class MyTankAlgorithm : public TankAlgorithm {
protected:
    int player_index_;
    int tank_index_;
    
    // Last received battle info data (copied, not referenced)
    size_t current_x_;
    size_t current_y_;
    Direction current_direction_;
    size_t remaining_shells_;
    int cooldown_timer_;
    
    // Board information
    size_t board_width_;
    size_t board_height_;
    
    // Game state
    size_t current_step_;
    size_t max_steps_;
    
    // Flag to track if we need to request battle info
    bool need_battle_info_;
    size_t last_info_step_;
    size_t info_request_interval_;
    
public:
    MyTankAlgorithm(int player_index, int tank_index);
    virtual ~MyTankAlgorithm() override = default;
    
    // This method must be implemented by derived classes
    ActionRequest getAction() override = 0;
    
    // Common update method that copies data from battle info
    virtual void updateBattleInfo(BattleInfo& info) override;
    
protected:
    // This method must be implemented by derived classes
    virtual void updateSpecificBattleInfo(BattleInfo& info) = 0;
    
    // Helper methods for tank actions
    bool shouldRequestBattleInfo() const;
    ActionRequest rotateToDirection(Direction target_direction) const;
    ActionRequest moveTowardsPosition(size_t target_x, size_t target_y) const;
    bool isPositionInFront(size_t x, size_t y) const;
    Direction getDirectionToPosition(size_t x, size_t y) const;
    bool canShoot() const;
};
