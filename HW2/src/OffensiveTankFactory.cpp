#include "../include/OffensiveFactory.h"
#include "../include/OffensiveTankAlgorithm.h"
#include "../common/TankAlgorithm.h"
#include <memory>

std::unique_ptr<TankAlgorithm> OffensiveTankFactory::create(int p, int t) const {
    // Creates a concrete instance (e.g., OffensiveTankAlgorithm)
    return std::make_unique<OffensiveTankAlgorithm>(p, t);
}