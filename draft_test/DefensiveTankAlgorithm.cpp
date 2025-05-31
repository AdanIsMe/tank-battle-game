#include "DefensiveTankAlgorithm.h"
#include <algorithm>
#include <limits>
#include <cmath>

DefensiveTankAlgorithm::DefensiveTankAlgorithm(int player_index, int tank_index)
    : MyTankAlgorithm(player_index, tank_index),
      in_danger_(false),
      danger_level_(0),
      has_escape_route_(false),
      action_counter_(0) {
}

ActionRequest DefensiveTankAlgorithm::getAction() {
    // Increment step counter
    current_step_++;
    
    // Every other step, request battle info
    if (shouldRequestBattleInfo()) {
        return ActionRequest::GetBattleInfo;
    }
    
    // Increment action counter
    action_counter_++;
    
    // Determine action based on current state
    return determineDefensiveAction();
}

void DefensiveTankAlgorithm::updateSpecificBattleInfo(BattleInfo& info) {
    // Cast to defensive battle info
    DefensiveBattleInfo& defensive_info = static_cast<DefensiveBattleInfo&>(info);
    
    // Copy danger assessment
    in_danger_ = defensive_info.inDanger();
    danger_level_ = defensive_info.getDangerLevel();
    danger_sources_ = defensive_info.getDangerSources();
    
    // Copy escape route
    has_escape_route_ = defensive_info.hasEscapeRoute();
    escape_route_ = defensive_info.getEscapeRoute();
    
    // Copy tanks to defend
    tanks_to_defend_ = defensive_info.getTanksToDefend();
    
    // Copy safe positions
    safe_positions_ = defensive_info.getSafePositions();
}

ActionRequest DefensiveTankAlgorithm::determineDefensiveAction() {
    // If in danger, try to escape
    if (in_danger_ && danger_level_ > 5) {
        return escapeFromDanger();
    }
    
    // If not in danger but have allies to defend, defend them
    if (!tanks_to_defend_.empty()) {
        return defendAllies();
    }
    
    // If not in danger and no allies to defend, move to a safe position
    if (!safe_positions_.empty()) {
        return moveToSafePosition();
    }
    
    // If all else fails, attack if possible
    return attackIfPossible();
}

ActionRequest DefensiveTankAlgorithm::escapeFromDanger() {
    // If we have an escape route, follow it
    if (has_escape_route_ && !escape_route_.empty()) {
        const auto& next_point = escape_route_[0];
        return moveTowardsPosition(next_point.first, next_point.second);
    }
    
    // If no escape route but in danger, move away from danger sources
    if (!danger_sources_.empty()) {
        // Calculate average danger position
        double avg_x = 0, avg_y = 0;
        for (const auto& source : danger_sources_) {
            avg_x += source.first;
            avg_y += source.second;
        }
        avg_x /= danger_sources_.size();
        avg_y /= danger_sources_.size();
        
        // Calculate direction away from danger
        int dx = static_cast<int>(current_x_) - static_cast<int>(avg_x);
        int dy = static_cast<int>(current_y_) - static_cast<int>(avg_y);
        
        // Normalize direction
        if (dx != 0 || dy != 0) {
            double length = std::sqrt(dx * dx + dy * dy);
            dx = static_cast<int>(dx / length);
            dy = static_cast<int>(dy / length);
        }
        
        // Calculate escape position
        size_t escape_x = (current_x_ + dx + board_width_) % board_width_;
        size_t escape_y = (current_y_ + dy + board_height_) % board_height_;
        
        // Move towards escape position
        return moveTowardsPosition(escape_x, escape_y);
    }
    
    // If all else fails, move randomly
    return (action_counter_ % 4 == 0) ? ActionRequest::MoveForward : 
           (action_counter_ % 4 == 1) ? ActionRequest::RotateRight45 :
           (action_counter_ % 4 == 2) ? ActionRequest::MoveForward :
           ActionRequest::RotateLeft45;
}

ActionRequest DefensiveTankAlgorithm::defendAllies() {
    // If no allies to defend, do nothing
    if (tanks_to_defend_.empty()) {
        return ActionRequest::DoNothing;
    }
    
    // Find the closest ally to defend
    const auto& ally = tanks_to_defend_[0];
    
    // Move towards ally
    return moveTowardsPosition(ally.first, ally.second);
}

ActionRequest DefensiveTankAlgorithm::moveToSafePosition() {
    // If no safe positions, do nothing
    if (safe_positions_.empty()) {
        return ActionRequest::DoNothing;
    }
    
    // Find the closest safe position
    const auto& safe_pos = safe_positions_[0];
    
    // Move towards safe position
    return moveTowardsPosition(safe_pos.first, safe_pos.second);
}

ActionRequest DefensiveTankAlgorithm::attackIfPossible() {
    // If we have danger sources and can shoot, shoot at them
    if (!danger_sources_.empty() && canShoot()) {
        const auto& target = danger_sources_[0];
        
        // If not facing target, rotate first
        Direction target_direction = getDirectionToPosition(target.first, target.second);
        if (target_direction != current_direction_) {
            return rotateToDirection(target_direction);
        }
        
        // If facing target, shoot
        return ActionRequest::Shoot;
    }
    
    // If no targets or can't shoot, rotate to scan surroundings
    return (action_counter_ % 2 == 0) ? ActionRequest::RotateRight45 : ActionRequest::DoNothing;
}
