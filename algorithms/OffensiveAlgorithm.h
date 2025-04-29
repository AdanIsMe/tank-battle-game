#ifndef OFFENSIVE_ALGORITHM_H
#define OFFENSIVE_ALGORITHM_H

#pragma once
#include "../include/TankAlgorithm.h"
#include "../include/GameBoard.h"
#include "../include/Direction.h"
#include "../include/Tank.h"
#include "../include/Shell.h"
#include "../include/Mine.h"
#include "../include/Wall.h"
#include <utility>
#include <vector>


class OffensiveAlgorithm : public TankAlgorithm  {
public:

    std::string getName() const override { return "OffensiveAlgorithm"; }

    // Public method to get the evasion action
    std::string getEvasionAction(const GameBoard& board, const Tank& tank) const;

    // Method to check if a shell is approaching the tank
    //bool isShellApproaching(const GameBoard& board, const Tank& myTank) const;

    // Method to find a safe direction for evasion
    //std::string findSafeDirection(const GameBoard& board, int x, int y, Direction dir, int playerId);

    // Method to check if the enemy tank is in sight
    bool enemyInSight(const GameBoard& board, const Tank& myTank, const Tank& enemyTank);

    // Helper method to check if the target is in the given direction
    bool isInDirection(const GameBoard& board, std::pair<int, int> start, std::pair<int, int> end, int dx, int dy);

    // Method to check if there's a clear path between two points
    bool hasClearPath(const GameBoard& board, std::pair<int, int> start, std::pair<int, int> end);

    // Method to check if the tank can shoot
    bool canShootNow(const Tank& tank);

    // Method to determine the best rotation direction to face the enemy
    Direction getBestRotationToEnemy(const GameBoard& board, const Tank& myTank, const Tank& enemyTank) const;

    // Method to get the rotation action needed to face a target direction
    std::string getRotationAction(const Tank& myTank, Direction targetDirection) const;

    // Method to check if a rotation would aim at the enemy tank
    bool isAimedAtEnemyAfterRotation(const GameBoard& board, const Tank& myTank, const Tank& enemyTank, std::string rotation);

    // get the chase action based on the enemy's position
    std::string getChaseAction(const GameBoard& board, const Tank& myTank, const Tank& enemyTank);

    std::string getNextAction(const GameBoard& board, int myTankId) override;

    private:

    mutable std::vector<std::pair<int, int>> cachedPath;
    mutable std::pair<int, int> cachedEnemyPos;
    // Helper function for pathfinding (breadth-first search)
    std::vector<std::pair<int, int>> bfsPath(const GameBoard& board, const std::pair<int, int>& start, const std::pair<int, int>& goal);
};

#endif // OFFENSIVE_ALGORITHM_H
