#include "../common/TankAlgorithmFactory.h"
#include "../common/TankAlgorithm.h"
#include <memory>

class OffensiveTankFactory : public TankAlgorithmFactory {
    public:
        std::unique_ptr<TankAlgorithm> create(int p, int t) const override;
};