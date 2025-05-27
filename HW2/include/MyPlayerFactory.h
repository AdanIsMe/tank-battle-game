#pragma once

#include "PlayerFactory.h"
#include "Player.h"
#include "OffensivePlayer.h"
#include "DefensivePlayer.h"

#include <memory>

class MyPlayerFactory : public PlayerFactory {
public:
    //~MyPlayerFactory() override = default;
    //~MyPlayerFactory() override {}
    //~MyPlayerFactory() noexcept(false) override {}


    ///////////// Still abstract – no create() implementation

    std::unique_ptr<Player> create(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const override;
};
