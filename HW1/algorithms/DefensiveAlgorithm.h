#pragma once
#include "../include/TankAlgorithm.h"
#include "../include/Direction.h"
#include "../include/GameBoard.h"
#include "../include/Direction.h"
#include "../include/Tank.h"
#include "../include/Shell.h"
#include "../include/Mine.h"
#include "../include/Wall.h"

class DefensiveAlgorithm : public TankAlgorithm {
public:
    std::string getNextAction(const GameBoard& board, int playerId) override;
    std::string getName() const override { return "DefensiveAlgorithm"; }
    
private:
    int consecutiveBackwardMoves = 0;
    bool isShellApproaching(const GameBoard& board, int x, int y, int playerId);
    bool canShootShell(const GameBoard& board, int x, int y, Direction dir, int playerId);
    
    
    bool isInImmediateDanger(const GameBoard& board, int x, int y);
    bool canSafelyShootOpponent(const GameBoard& board, std::shared_ptr<Tank> tank);
    bool hasClearShot(const GameBoard& board, int fromX, int fromY, int toX, int toY);
    std::string getDefaultSafeMove(const GameBoard& board, std::shared_ptr<Tank> tank);
    bool isTileSafe(const GameBoard& board, int tx, int ty);
    
    std::string handleImmediateDanger(const GameBoard& board, std::shared_ptr<Tank> tank);
    
};