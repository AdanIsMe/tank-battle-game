#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "player.h"
#include "gameBoard.h"
#include <vector>

class GameManager {
private:
    int gameSteps;
    bool shellsLeft;
    bool gameRunning;
    GameBoard board;
    std::vector<Shell> activeShells;
    Player player1;
    Player player2;

public:
    GameManager(int width, int height);
    
    void initializeGame();
    void play();
    void updateGameState();
    void checkWinConditions();
    void displayGameState() const;
};

#endif // GAMEMANAGER_H