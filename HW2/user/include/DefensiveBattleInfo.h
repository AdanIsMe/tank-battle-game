#include <vector>

class DefensiveBattleInfo : public MyBattleInfo{
    private:
        //list of immediate threats : list of Shells
        std::vector<Shell> immediate_threats;
        std::vector<Shell> all_threats;
        bool is_under_immediate_threat;       // True if satView revealed that shells are very close
        Position suggested_retreat_position; // Best defensive position to move to

    public:
        ~DefensiveBattleInfo() override = default;

};
