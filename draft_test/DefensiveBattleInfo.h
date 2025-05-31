#pragma once
#include "MyBattleInfo.h"
#include <vector>
#include <utility>

class DefensiveBattleInfo : public MyBattleInfo {
private:
    // Danger assessment
    bool in_danger_;
    int danger_level_;  // 0-10 scale, 10 being highest danger
    std::vector<std::pair<size_t, size_t>> danger_sources_;  // Positions of threats
    
    // Escape planning
    bool has_escape_route_;
    std::vector<std::pair<size_t, size_t>> escape_route_;
    
    // Defensive targets (tanks to defend)
    std::vector<std::pair<size_t, size_t>> tanks_to_defend_;
    
    // Safe zones
    std::vector<std::pair<size_t, size_t>> safe_positions_;
    
public:
    DefensiveBattleInfo();
    ~DefensiveBattleInfo() override = default;
    
    // Danger getters
    bool inDanger() const { return in_danger_; }
    int getDangerLevel() const { return danger_level_; }
    const std::vector<std::pair<size_t, size_t>>& getDangerSources() const {
        return danger_sources_;
    }
    
    // Escape getters
    bool hasEscapeRoute() const { return has_escape_route_; }
    const std::vector<std::pair<size_t, size_t>>& getEscapeRoute() const {
        return escape_route_;
    }
    
    // Defense getters
    const std::vector<std::pair<size_t, size_t>>& getTanksToDefend() const {
        return tanks_to_defend_;
    }
    
    // Safe zone getters
    const std::vector<std::pair<size_t, size_t>>& getSafePositions() const {
        return safe_positions_;
    }
    
    // Danger setters
    void setDanger(bool in_danger, int level = 5);
    void addDangerSource(size_t x, size_t y);
    void clearDangerSources();
    
    // Escape setters
    void setHasEscapeRoute(bool has_route);
    void addEscapePoint(size_t x, size_t y);
    void clearEscapeRoute();
    
    // Defense setters
    void addTankToDefend(size_t x, size_t y);
    void clearTanksToDefend();
    
    // Safe zone setters
    void addSafePosition(size_t x, size_t y);
    void clearSafePositions();
};
