class OffensiveTankAlgorithm : public MyTankAlgorithm {
    public:
        OffensiveTankAlgorithm(int player_index, int tank_index);
        ~OffensiveTankAlgorithm() override = default;

        ActionRequest getAction() override;
        void updateBattleInfo(BattleInfo& info) override; // update Offensive info
};