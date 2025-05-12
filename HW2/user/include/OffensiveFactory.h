class OffensiveFactory : public TankAlgorithmFactory {
    public:
        std::unique_ptr<TankAlgorithm> create(int p, int t) const override;
};