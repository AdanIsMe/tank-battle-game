#pragma once
#include "../include/TankAlgorithm.h"
#include "../include/GameBoard.h"
#include "../include/Direction.h"
#include "../include/Tank.h"
#include "../include/Shell.h"
#include "../include/Mine.h"
#include "../include/Wall.h"

#include <queue>
#include <utility>
#include <unordered_map>
#include <unordered_set>

class ChaseAlgorithm : public TankAlgorithm {
public:
    std::string getNextAction(const GameBoard& board, int playerId) override;
    std::string getName() const override { return "ChaseAlgorithm"; }
    
private:
    std::pair<int, int> findTargetPosition(const GameBoard& board, int playerId);
    std::vector<std::pair<int, int>> findPath(const GameBoard& board, 
                                            const std::pair<int, int>& start, 
                                            const std::pair<int, int>& target);
    bool isPositionSafe(const GameBoard& board, int x, int y, int playerId);
    std::string getMoveDirection(const std::pair<int, int>& current, 
                                const std::pair<int, int>& next, 
                                Direction currentDir);
    
    std::unordered_map<int, std::pair<int, int>> lastTargetPositions;
    std::unordered_map<int, std::vector<std::pair<int, int>>> currentPaths;
};