#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "player.h"
#include "gameBoard.h"
#include "wall.h"
#include "mine.h"
#include "shell.h"

#include <vector>
#include <iostream>
#include <algorithm>


class GameManager {
private:
    int game_steps;
    bool zero_shells_left;
    bool game_running;
    GameBoard& board;
    std::vector<std::unique_ptr<Shell>> active_shells;
    Player player1;
    Player player2;
    int tie_counter;

public:
    GameManager(GameBoard& board,int x1, int y1, int x2, int y2);
    
    void initializeGame(int x1, int y1, int x2, int y2);
    void play();
    void updateGameState();
    void checkWinConditions();
    void displayGameState() const;

    void handleShellCollision(Shell& shell, GameObject* obj);
    void handleTankCollision(Tank& tank, GameObject* obj);
    bool legalMove(int move, int player);

    void addShell(const Shell& shell);
};

#endif // GAMEMANAGER_H