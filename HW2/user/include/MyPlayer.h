class MyPlayer : public Player {
    protected:
        int player_index;
        size_t board_rows;
        size_t boards_cols;
        size_t max_steps;
        size_t num_shells;
    public:
        MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
        
        //virtual ~MyPlayer(); ?
        //void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};
