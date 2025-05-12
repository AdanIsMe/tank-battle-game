#pragma once
#include "MyBattleInfo.h"

class OffensiveBattleInfo : public MyBattleInfo {
    private:
        // Attack planning
        std::vector<std::pair<size_t, size_t>> enemy_prediction_paths; // possibly using partial BFS

        // Offensive-specific fields
        std::vector<std::pair<size_t, size_t>> attack_vectors;
        std::vector<std::pair<size_t, size_t>> flanking_routes;
        int optimal_attack_range;

    public:
        // Offensive update methods
        void updateAttackOpportunities(const SatelliteView& view) override;
        void predictEnemyMovement();
        
        // Offensive queries
        bool hasClearShot() const {
            return !attack_vectors.empty(); //DONT IMPLEMENT IN HEADER, THIS IS JUST A PLACEHOLDER
        }
        const std::vector<std::pair<size_t, size_t>>& getFlankingOptions() const {
            return flanking_routes;
        }
};