#pragma once

#include "MyPlayerFactory.h"
#include "OffensivePlayer.h"

class OffensivePlayerFactory : public MyPlayerFactory {
public:
    std::unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                   size_t max_steps, size_t num_shells) const override {
        return std::make_unique<OffensivePlayer>(player_index, x, y, max_steps, num_shells);
    }
};
