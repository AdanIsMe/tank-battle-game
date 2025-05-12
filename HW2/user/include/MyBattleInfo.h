
#include <vector>

class MyBattleInfo : public BattleInfo {
    protected:
        // Battlefield info
        size_t board_rows;
        size_t board_cols
        
        // Tank information
        MyTankAlgorithm my_tank;
        std::vector<MyTankAlgorithm> enemy_tanks;
        
        // Projectiles
        std::vector<Shell> incoming_shells;
        int nearest_shell_distance;          // Distance of closest threatening shell

        //std::vector<Shell> outgoing_shells;
        
        // Environment
        std::vector<std::pair<size_t, size_t>> mine_positions;
        std::vector<std::pair<size_t, size_t>> wall_positions;
        std::vector<std::pair<size_t, size_t>> weak_walls;
        
        // Calculated data (updated by Player)
        std::vector<std::pair<size_t, size_t>> safe_positions;
        std::vector<std::pair<size_t, size_t>> dangerous_positions;
        int threat_level;
        
        // Game state
        int current_game_step;
        int max_game_steps;

        // Mine awareness
        std::vector<Position> nearby_mines;     // Mines within dangerous proximity
        Position closest_mine;                  // Closest mine position

        // Tank status
        int remaining_shells;                   // Current ammunition count
        int steps_until_can_shoot;                // Cooldown until next shot

        
        // Update methods called by Player
       virtual void updateSafePositions(const SatelliteView& view) = 0;
        virtual void updateThreatAssessment() = 0;
};