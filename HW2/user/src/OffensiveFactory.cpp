#include "../include/OffensiveFactory.h"

std::unique_ptr<TankAlgorithm> create(int p, int t) const override {
    // Creates a concrete instance (e.g., OffensiveTankAlgorithm)
    return std::make_unique<OffensiveTankAlgorithm>(p, t);
}