#pragma once
#include "../include/TankAlgorithm.h"
#include "../include/Direction.h"
#include "../include/GameBoard.h"
#include "../include/Direction.h"
#include "../include/Tank.h"
#include "../include/Shell.h"
//#include "../include/Mine.h"
//#include "../include/Wall.h"

class DefensiveAlgorithm : public TankAlgorithm {
public:
    std::string getNextAction(const GameBoard& board, int playerId) override;
    std::string getName() const override { return "DefensiveAlgorithm"; }
    
private:
    bool isShellApproaching(const GameBoard& board, int x, int y, int playerId);
    std::string findSafeDirection(const GameBoard& board, int x, int y, Direction currentDir, int playerId);
};