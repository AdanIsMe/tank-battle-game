#include "OffensiveTankAlgorithm.h"
#include <algorithm>
#include <cmath>

OffensiveTankAlgorithm::OffensiveTankAlgorithm(int player_index, int tank_index)
    : MyTankAlgorithm(player_index, tank_index),
      has_target_(false),
      target_x_(0),
      target_y_(0),
      under_threat_(false),
      can_hit_target_(false),
      shots_needed_(0),
      action_counter_(0) {
}

ActionRequest OffensiveTankAlgorithm::getAction() {
    // Increment step counter
    current_step_++;
    
    // Every other step, request battle info
    if (shouldRequestBattleInfo()) {
        return ActionRequest::GetBattleInfo;
    }
    
    // Increment action counter
    action_counter_++;
    
    // Determine action based on current state
    return determineOffensiveAction();
}

void OffensiveTankAlgorithm::updateSpecificBattleInfo(BattleInfo& info) {
    // Cast to offensive battle info
    OffensiveBattleInfo& offensive_info = static_cast<OffensiveBattleInfo&>(info);
    
    // Copy target data
    has_target_ = offensive_info.hasTarget();
    if (has_target_) {
        target_x_ = offensive_info.getTargetX();
        target_y_ = offensive_info.getTargetY();
    }
    
    // Copy path data
    path_to_target_ = offensive_info.getPathToTarget();
    
    // Copy threat data
    under_threat_ = offensive_info.isUnderThreat();
    incoming_shells_ = offensive_info.getIncomingShells();
    
    // Copy shooting opportunity data
    can_hit_target_ = offensive_info.canHitTarget();
    shots_needed_ = offensive_info.getShotsNeeded();
}

ActionRequest OffensiveTankAlgorithm::determineOffensiveAction() {
    // If under threat, handle it first
    if (under_threat_) {
        return handleThreat();
    }
    
    // If we have a target and can hit it, attack
    if (has_target_ && can_hit_target_ && canShoot()) {
        return attackTarget();
    }
    
    // If we have a path to target, follow it
    if (has_target_ && !path_to_target_.empty()) {
        return followPathToTarget();
    }
    
    // If we have a target but no path, rotate towards it
    if (has_target_) {
        Direction target_direction = getDirectionToPosition(target_x_, target_y_);
        return rotateToDirection(target_direction);
    }
    
    // Default action: rotate to scan surroundings
    return (action_counter_ % 2 == 0) ? ActionRequest::RotateRight45 : ActionRequest::DoNothing;
}

ActionRequest OffensiveTankAlgorithm::handleThreat() {
    // If no incoming shells, just move randomly
    if (incoming_shells_.empty()) {
        return ActionRequest::MoveForward;
    }
    
    // Find the closest incoming shell
    size_t closest_shell_x = 0;
    size_t closest_shell_y = 0;
    double min_distance = std::numeric_limits<double>::max();
    
    for (const auto& shell : incoming_shells_) {
        double dx = static_cast<int>(shell.first) - static_cast<int>(current_x_);
        double dy = static_cast<int>(shell.second) - static_cast<int>(current_y_);
        double distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance < min_distance) {
            min_distance = distance;
            closest_shell_x = shell.first;
            closest_shell_y = shell.second;
        }
    }
    
    // Move perpendicular to the incoming shell
    int dx = static_cast<int>(closest_shell_x) - static_cast<int>(current_x_);
    int dy = static_cast<int>(closest_shell_y) - static_cast<int>(current_y_);
    
    // Calculate perpendicular direction
    Direction perp_direction;
    if (std::abs(dx) > std::abs(dy)) {
        // Shell is coming horizontally, move vertically
        perp_direction = (action_counter_ % 2 == 0) ? Direction::UP : Direction::DOWN;
    } else {
        // Shell is coming vertically, move horizontally
        perp_direction = (action_counter_ % 2 == 0) ? Direction::LEFT : Direction::RIGHT;
    }
    
    // Rotate to perpendicular direction if needed
    if (perp_direction != current_direction_) {
        return rotateToDirection(perp_direction);
    }
    
    // Move forward in perpendicular direction
    return ActionRequest::MoveForward;
}

ActionRequest OffensiveTankAlgorithm::attackTarget() {
    // If not facing target, rotate first
    Direction target_direction = getDirectionToPosition(target_x_, target_y_);
    if (target_direction != current_direction_) {
        return rotateToDirection(target_direction);
    }
    
    // If facing target and can shoot, shoot
    if (canShoot()) {
        return ActionRequest::Shoot;
    }
    
    // If can't shoot (cooldown), move towards target
    return followPathToTarget();
}

ActionRequest OffensiveTankAlgorithm::followPathToTarget() {
    // If path is empty, do nothing
    if (path_to_target_.empty()) {
        return ActionRequest::DoNothing;
    }
    
    // Get next point in path
    const auto& next_point = path_to_target_[0];
    
    // Move towards next point
    return moveTowardsPosition(next_point.first, next_point.second);
}
