// Intermediate base algorithm class
class MyTankAlgorithm : public TankAlgorithm {
    protected:
        int player_index;
        int tank_index;
        
    public:
        MyTankAlgorithm(int player_index, int tank_index);
        
        // Pure virtual - forces derived classes to implement
        virtual void updateBattleInfo(BattleInfo& info) = 0;
        
        // Keep getAction() pure virtual as well
        virtual ActionRequest getAction() = 0;
        
        virtual ~MyTankAlgorithm() = default;
};