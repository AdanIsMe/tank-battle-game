#pragma once

#include "MyPlayer.h"

class DefensivePlayer : public MyPlayer {
public:
    DefensivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
    ~DefensivePlayer() override = default;

    // Override Player interface methods here
    // Example:
    //void makeMove() override;
};
