#include "OffensiveBattleInfo.h"

OffensiveBattleInfo::OffensiveBattleInfo()
    : has_target_(false),
      target_x_(0),
      target_y_(0),
      target_direction_(Direction::NONE),
      under_threat_(false),
      can_hit_target_(false),
      shots_needed_(0) {
}

void OffensiveBattleInfo::setTarget(size_t x, size_t y, Direction direction) {
    target_x_ = x;
    target_y_ = y;
    target_direction_ = direction;
    has_target_ = true;
}

void OffensiveBattleInfo::clearTarget() {
    has_target_ = false;
}

void OffensiveBattleInfo::addPathPoint(size_t x, size_t y) {
    path_to_target_.push_back({x, y});
}

void OffensiveBattleInfo::clearPath() {
    path_to_target_.clear();
}

void OffensiveBattleInfo::setUnderThreat(bool under_threat) {
    under_threat_ = under_threat;
}

void OffensiveBattleInfo::addIncomingShell(size_t x, size_t y) {
    incoming_shells_.push_back({x, y});
}

void OffensiveBattleInfo::clearIncomingShells() {
    incoming_shells_.clear();
}

void OffensiveBattleInfo::setCanHitTarget(bool can_hit) {
    can_hit_target_ = can_hit;
}

void OffensiveBattleInfo::setShotsNeeded(int shots) {
    shots_needed_ = shots;
}
