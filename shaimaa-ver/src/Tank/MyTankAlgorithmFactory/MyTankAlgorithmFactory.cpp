#include "MyTankAlgorithmFactory.h"
#include "../OffensiveTankAlgorithm/OffensiveTankAlgorithm.h"
#include "../DefensiveTankAlgorithm/DefensiveTankAlgorithm.h"
#include <memory>

std::unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {
    // Create different algorithm types based on player_index
    if (player_index == 1) {
        return std::make_unique<OffensiveTankAlgorithm>(player_index, tank_index);
    } else {
        return std::make_unique<DefensiveTankAlgorithm>(player_index, tank_index);
    }
}
