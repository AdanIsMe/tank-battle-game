#pragma once

#include "MyPlayer.h"

class OffensivePlayer : public MyPlayer {
public:
    OffensivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
    ~OffensivePlayer() override = default;

    // Override Player interface methods here
    // Example:
    //void makeMove() override;
};
