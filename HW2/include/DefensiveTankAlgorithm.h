#include "MyTankAlgorithm.h"
#include "../common/ActionRequest.h"
#include "../common/BattleInfo.h"

class DefensiveTankAlgorithm : public MyTankAlgorithm {
    public:
        DefensiveTankAlgorithm(int player_index, int tank_index);
        ~DefensiveTankAlgorithm() override = default;

        ActionRequest getAction() override;
        void updateBattleInfo(BattleInfo& info) override;
};