#include "../include/DefensiveFactory.h"

std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override {
    // Creates a concrete instance (e.g., DefensiveTankAlgorithm)
    return std::make_unique<DefensiveTankAlgorithm>(player_index, tank_index);
}