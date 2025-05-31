#include "MyPlayerFactory.h"
#include "OffensivePlayer.h"
#include "DefensivePlayer.h"
#include <memory>

std::unique_ptr<Player> MyPlayerFactory::create(int player_index, size_t x, size_t y, 
                                              size_t max_steps, size_t num_shells) const {
    // Create different player types based on player_index
    if (player_index == 1) {
        return std::make_unique<OffensivePlayer>(player_index, x, y, max_steps, num_shells);
    } else {
        return std::make_unique<DefensivePlayer>(player_index, x, y, max_steps, num_shells);
    }
}
