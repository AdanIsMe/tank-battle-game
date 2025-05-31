#include "DefensiveBattleInfo.h"

DefensiveBattleInfo::DefensiveBattleInfo()
    : in_danger_(false),
      danger_level_(0),
      has_escape_route_(false) {
}

void DefensiveBattleInfo::setDanger(bool in_danger, int level) {
    in_danger_ = in_danger;
    danger_level_ = level;
}

void DefensiveBattleInfo::addDangerSource(size_t x, size_t y) {
    danger_sources_.push_back({x, y});
}

void DefensiveBattleInfo::clearDangerSources() {
    danger_sources_.clear();
}

void DefensiveBattleInfo::setHasEscapeRoute(bool has_route) {
    has_escape_route_ = has_route;
}

void DefensiveBattleInfo::addEscapePoint(size_t x, size_t y) {
    escape_route_.push_back({x, y});
}

void DefensiveBattleInfo::clearEscapeRoute() {
    escape_route_.clear();
}

void DefensiveBattleInfo::addTankToDefend(size_t x, size_t y) {
    tanks_to_defend_.push_back({x, y});
}

void DefensiveBattleInfo::clearTanksToDefend() {
    tanks_to_defend_.clear();
}

void DefensiveBattleInfo::addSafePosition(size_t x, size_t y) {
    safe_positions_.push_back({x, y});
}

void DefensiveBattleInfo::clearSafePositions() {
    safe_positions_.clear();
}
