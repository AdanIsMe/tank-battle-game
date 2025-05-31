#pragma once
#include "../../common/SatelliteView.h"
#include <vector>
#include <cstddef> // for size_t

class GameSatelliteView : public SatelliteView {
private:
    const std::vector<std::vector<char>>& board_;
    size_t requesting_tank_x_;
    size_t requesting_tank_y_;
    int requesting_player_index_;
    
public:
    GameSatelliteView(const std::vector<std::vector<char>>& board, 
                     size_t tank_x, size_t tank_y, int player_index);
    ~GameSatelliteView() override = default;
    
    char getObjectAt(size_t x, size_t y) const override;
};
