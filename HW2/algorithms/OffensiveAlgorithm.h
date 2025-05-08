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
    // Method to find a safe direction for evasion
    std::string findSafeDirection(const GameBoard& board, int x, int y, Direction current_dir, int player_id) const;
    bool canShootShell(const GameBoard& board, int x, int y, Direction dir) const;
    // Method to check if the enemy tank is in sight
    bool enemyInSight(const GameBoard& board, const Tank& my_tank, const Tank& enemy_tank);

    // Method to check if there's a clear path between two points
    bool hasClearPath(const GameBoard& board, std::pair<int, int> start, std::pair<int, int> end);

    // Method to check if the tank can shoot
    bool canShootNow(const Tank& tank)const;

    // Method to determine the best rotation direction to face the enemy
    std::pair<bool, Direction> getBestRotationToEnemy(
        const GameBoard& board,
        const Tank& my_tank,
        const Tank& enemy_tank);
    // Method to get the rotation action needed to face a target direction
    std::string getRotationAction(const Tank& my_tank, Direction target_direction) const;

    // Method to check if a rotation would aim at the enemy tank
    bool isAimedAtEnemyAfterRotation(const GameBoard& board, const Tank& my_tank, const Tank& enemy_tank, std::string rotation);

    // get the chase action based on the enemy's position
    std::string getChaseAction(const GameBoard& board, const Tank& my_tank, const Tank& enemy_tank);

    std::string getNextAction(const GameBoard& board, int my_tank_id) override;

    private:

    mutable std::vector<std::pair<int, int>> cached_path;
    mutable std::pair<int, int> cached_enemy_pos;
    // Helper function for pathfinding (breadth-first search)
    std::vector<std::pair<int, int>> bfsPath(
        const GameBoard& board,
        const Tank& tank,
        const std::pair<int, int>& goal,
        int max_steps = 5);
    bool isTileSafe(const GameBoard& board, int tx, int ty) const;
};

#endif // OFFENSIVE_ALGORITHM_H
