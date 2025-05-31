#include "MyTankAlgorithm.h"
#include <cmath>
#include <unordered_map>

MyTankAlgorithm::MyTankAlgorithm(int player_index, int tank_index)
    : player_index_(player_index),
      tank_index_(tank_index),
      current_x_(0),
      current_y_(0),
      current_direction_((player_index == 1) ? Direction::LEFT : Direction::RIGHT),
      remaining_shells_(0),
      cooldown_timer_(0),
      board_width_(100),
      board_height_(100),
      current_step_(0),
      max_steps_(5000),
      need_battle_info_(true),
      last_info_step_(0),
      info_request_interval_(2) {
}

void MyTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    // Reset the flag since we're getting fresh info
    need_battle_info_ = false;
    last_info_step_ = current_step_;
    
    // Cast to our custom battle info type
    MyBattleInfo& my_info = static_cast<MyBattleInfo&>(info);
    
    // Copy data from the battle info (not storing references)
    current_x_ = my_info.getCurrentX();
    current_y_ = my_info.getCurrentY();
    current_direction_ = my_info.getCurrentDirection();
    remaining_shells_ = my_info.getRemainingShells();
    cooldown_timer_ = my_info.getCooldownTimer();
    
    // Copy board information
    board_width_ = my_info.getBoardWidth();
    board_height_ = my_info.getBoardHeight();
    
    // Copy game state
    current_step_ = my_info.getCurrentStep();
    max_steps_ = my_info.getMaxSteps();
    
    // Derived classes will handle their specific battle info
    updateSpecificBattleInfo(info);
}

bool MyTankAlgorithm::shouldRequestBattleInfo() const {
    // Request battle info if:
    // 1. We haven't received info yet
    // 2. It's been too long since last update
    // 3. We're in a potentially dangerous situation
    return need_battle_info_ || 
           (current_step_ - last_info_step_ >= info_request_interval_);
}

ActionRequest MyTankAlgorithm::rotateToDirection(Direction target_direction) const {
    if (target_direction == current_direction_) {
        return ActionRequest::DoNothing;
    }

    // Convert enums to integers [0..7]
    int current_idx = static_cast<int>(current_direction_);
    int target_idx = static_cast<int>(target_direction);

    // Calculate difference modulo 8
    int diff = (target_idx - current_idx + 8) % 8;

    // Map rotation diff to action
    switch (diff) {
        case 1: return ActionRequest::RotateRight45;
        case 2: return ActionRequest::RotateRight90;
        case 3: return ActionRequest::RotateRight90;  // Or special 135°
        case 4: return ActionRequest::RotateRight90;  // 180° – you can double-rotate if needed
        case 5: return ActionRequest::RotateLeft90;   // equivalent to right 135°
        case 6: return ActionRequest::RotateLeft90;
        case 7: return ActionRequest::RotateLeft45;
        default: return ActionRequest::DoNothing;
    }
}

ActionRequest MyTankAlgorithm::moveTowardsPosition(size_t target_x, size_t target_y) const {
    Direction target_direction = getDirectionToPosition(target_x, target_y);

    if (target_direction != current_direction_) {
        // Determine shortest rotation towards the target direction
        // You can implement a function to compute the minimal rotation (left/right)
        return rotateToDirection(target_direction);
    }

    return ActionRequest::MoveForward;
}

bool MyTankAlgorithm::isPositionInFront(size_t x, size_t y) const {
    int dx = static_cast<int>(x) - static_cast<int>(current_x_);
    int dy = static_cast<int>(y) - static_cast<int>(current_y_);

    // Handle wrap-around for board edges (assumes toroidal map)
    if (dx > static_cast<int>(board_width_) / 2) dx -= board_width_;
    if (dx < -static_cast<int>(board_width_) / 2) dx += board_width_;
    if (dy > static_cast<int>(board_height_) / 2) dy -= board_height_;
    if (dy < -static_cast<int>(board_height_) / 2) dy += board_height_;

    std::pair<int, int> movement = DirectionUtil::getMovement(current_direction_);
    return (dx == movement.first && dy == movement.second) || 
           (movement.first != 0 && dx * movement.first > 0 && (movement.second == 0 || dy * movement.second > 0)) ||
           (movement.second != 0 && dy * movement.second > 0 && (movement.first == 0 || dx * movement.first > 0));
}

Direction MyTankAlgorithm::getDirectionToPosition(size_t x, size_t y) const {
    int dx = static_cast<int>(x) - static_cast<int>(current_x_);
    int dy = static_cast<int>(y) - static_cast<int>(current_y_);

    // Handle wrap-around for toroidal board
    if (dx > static_cast<int>(board_width_) / 2) dx -= board_width_;
    if (dx < -static_cast<int>(board_width_) / 2) dx += board_width_;
    if (dy > static_cast<int>(board_height_) / 2) dy -= board_height_;
    if (dy < -static_cast<int>(board_height_) / 2) dy += board_height_;

    return DirectionUtil::getDirectionFromDelta(dx, dy);
}

bool MyTankAlgorithm::canShoot() const {
    return remaining_shells_ > 0 && cooldown_timer_ == 0;
}
