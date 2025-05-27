class DefensiveFactory : public TankAlgorithmFactory {
    public:
        std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override;
};
