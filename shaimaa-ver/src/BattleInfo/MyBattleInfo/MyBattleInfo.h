#pragma once
#include "../../common/BattleInfo.h"
#include "../../Game/Direction/Direction.h"
#include <cstddef>
//#include <cstdint>

// Base class for battle info implementations
class MyBattleInfo : public BattleInfo {
protected:
    // Tank's current state
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
    
public:
    MyBattleInfo();
    virtual ~MyBattleInfo() override = default;
    
    // Getters for tank state
    size_t getCurrentX() const { return current_x_; }
    size_t getCurrentY() const { return current_y_; }
    Direction getCurrentDirection() const { return current_direction_; }
    size_t getRemainingShells() const { return remaining_shells_; }
    int getCooldownTimer() const { return cooldown_timer_; }
    
    // Getters for board information
    size_t getBoardWidth() const { return board_width_; }
    size_t getBoardHeight() const { return board_height_; }
    
    // Getters for game state
    size_t getCurrentStep() const { return current_step_; }
    size_t getMaxSteps() const { return max_steps_; }
    
    // Setters
    void setCurrentPosition(size_t x, size_t y);
    void setCurrentDirection(Direction direction);
    void setRemainingShells(size_t shells);
    void setCooldownTimer(int timer);
    void setBoardDimensions(size_t width, size_t height);
    void setGameState(size_t current_step, size_t max_steps);
};
