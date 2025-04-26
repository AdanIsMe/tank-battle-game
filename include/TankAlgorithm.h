#pragma once
#include "GameBoard.h"
#include <string>

class TankAlgorithm {
public:
    virtual ~TankAlgorithm() = default;
    virtual std::string getNextAction(const GameBoard& board, int playerId) = 0;
    virtual std::string getName() const = 0;
};