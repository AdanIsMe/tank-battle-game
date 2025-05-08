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
    std::string getNextAction(const GameBoard& board, int player_id) override;
    std::string getName() const override { return "DefensiveAlgorithm"; }
    
private:
    int consecutive_backward_moves = 0;

    bool isShellApproaching(const GameBoard& board, int x, int y, int player_id);
    bool canShootShell(const GameBoard& board, int x, int y, Direction dir, int player_id);
    
    bool isInImmediateDanger(const GameBoard& board, int x, int y);
    bool canSafelyShootOpponent(const GameBoard& board, std::shared_ptr<Tank> tank);
    bool hasClearShot(const GameBoard& board, int from_x, int from_y, int to_x, int to_y);
    std::string getDefaultSafeMove(const GameBoard& board, std::shared_ptr<Tank> tank);
    bool isTileSafe(const GameBoard& board, int tx, int ty);
    
    std::string handleImmediateDanger(const GameBoard& board, std::shared_ptr<Tank> tank);
    
};