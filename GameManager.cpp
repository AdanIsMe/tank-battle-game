#include "GameManager.h"
#include <iostream>

GameManager::GameManager(int width, int height) 
    : board(width, height), gameSteps(0), shellsLeft(40), gameRunning(true) {
    initializeGame();
}

void GameManager::initializeGame() {
    // Initialize players' tanks
    board.placeObject(std::make_shared<Tank>(player1.myTank), 0, 0);
    board.placeObject(std::make_shared<Tank>(player2.myTank), board.getHeight()-1, board.getWidth()-1);
    
    // Other initialization logic
}

void GameManager::play() {
    while (gameRunning && shellsLeft > 0) {
        gameSteps++;
        
        // Process player actions
        player1.myTank.action(player1.nextMove());
        player2.myTank.action(player2.nextMove());
        
        // Update all game objects
        updateGameState();
        
        // Check for win conditions
        checkWinConditions();
        
        // Display current state
        displayGameState();
    }
}

void GameManager::updateGameState() {
    // Update shells
    for (auto& shell : activeShells) {
        shell.updatePosition();
        // Check for collisions
        if (board.getObjectAt(shell.getX(), shell.getY()) != nullptr) {
            // Handle collision
            //if wall: get damaged 
            //if tank: get destroyed and change state is alive to false
            //if two shells remove both
            board.removeObject(shell.getX(), shell.getY());
            shellsLeft--;
        }
    }
    
    // Remove destroyed objects
    activeShells.erase(std::remove_if(activeShells.begin(), activeShells.end(),
        [](const Shell& s) { return s.shouldRemove(); }), activeShells.end());
}

void GameManager::checkWinConditions() {
    // Check if players have tanks remaining
    bool p1Alive = player1.myTank.isAlive;
    bool p2Alive = player2.myTank.isAlive;
    
    if (!p1Alive || !p2Alive) {
        gameRunning = false;
        std::cout << "Game over! Winner: " << (p1Alive ? "Player 1" : "Player 2") << std::endl;
    }
    else if (player1.my_shells && player2.my_shells) {
        gameRunning = false;
        std::cout << "Game over! Draw - no shells left." << std::endl;
    }
}

void GameManager::displayGameState() const {
    std::cout << "\n=== Game State ===" << std::endl;
    std::cout << "Step: " << gameSteps << std::endl;
    std::cout << "Shells left: " << shellsLeft << std::endl;
    board.displayBoard();
}