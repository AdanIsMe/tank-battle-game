#pragma once
#include "GameBoard.h"
#include "Direction.h"
#include "Shell.h"
#include "Tank.h"
#include <string>

class TankAlgorithm {
public:
    virtual ~TankAlgorithm() = default;
    virtual std::string getNextAction(const GameBoard& board, int player_id) = 0;
    virtual std::string getName() const = 0;
    bool isShellApproaching(const GameBoard& board, const Tank& my_tank) const;
    std::string findSafeDirection(const GameBoard& board, int player_id) const;

    // Helper method to check if the target is in the given direction
    bool isInDirection(const GameBoard& board, std::pair<int, int> start, std::pair<int, int> end, int dx, int dy);
};