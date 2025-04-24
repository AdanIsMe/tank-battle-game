#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "player.h"
#include "gameBoard.h"
#include <vector>

class GameManager {
private:
    int gameSteps;
    bool zeroShellsLeft;
    bool gameRunning;
    GameBoard board;
    std::vector<Shell> activeShells;
    Player player1;
    Player player2;
    int tieCounter;

public:
    GameManager(int width, int height);
    
    void initializeGame();
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