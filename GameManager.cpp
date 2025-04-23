#include "GameManager.h"
#include <iostream>
#include "gameObject.h"
#include "wall.h"

GameManager::GameManager(int width, int height) 
    : board(width, height), gameSteps(0), shellsLeft(true), gameRunning(true) {
    initializeGame();
}


//////////////////////////////////////////////////////////////////////////////////////

void GameManager::initializeGame() {
    // Initialize players' tanks - now using make_unique
    board.placeObject(std::make_unique<Tank>(player1.myTank), 0, 0);
    board.placeObject(std::make_unique<Tank>(player2.myTank), board.getHeight()-1, board.getWidth()-1);
    
    // Other initialization logic
}

//////////////////////////////////////////////////////////////////////////////////////

void GameManager::play() {
    while (gameRunning && shellsLeft) {
        /// note to me: if shellsleft is false start counting to 40(game steps)
        //if no one wins stop the game and declare a tie 
        gameSteps++;
        
        // Process player actions
        int move1 = player1.nextMove();
        int move2 = player2.nextMove();

        if(!legalMove(move1,1)){
            //if the move is ilegal ignore it
            std::cout << "bad step" << std::endl;
        }
        else{
            //if it's a legal move perform it
            player1.myTank.action(move1);
        }

        if(!legalMove(move2,2)){
            //if the move is ilegal ignore it
            std::cout << "bad step" << std::endl;
        }
        else{
            //if it's a legal move perform it
            player2.myTank.action(move2);
        }

        ///if one of the actions is shoot we should add the shell to active shells
        
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
    //should add active shells
    for (auto& shell : activeShells) {
        shell.updatePosition();
        GameObject* obj = board.getObjectAt(shell.getX(), shell.getY());
        if (obj != nullptr) {
            // Handle collision
            //first: update that the shell should be removed
            shell.remove = true;
            //if wall: get damaged 
            if(Wall *wall = dynamic_cast<Wall*>(obj)){
                if(wall->weaken()){
                    board.removeObject(wall->getX(), wall->getY());
                    //delete wall object
                }
            }
            //if tank: get destroyed and change state is alive to false
            if(Tank *tank = dynamic_cast<Tank*>(obj)){
                if(wall->weaken()){
                    board.removeObject(tank->getX(), tank->getY());
                    //delete wall object
                }
            }
            //if two shells remove both

            board.removeObject(shell.getX(), shell.getY());

            //check if both tanks used all thier artillery 
            shellsLeft = player1.my_shells || player2.my_shells;
        }
    
    }
    
    // Remove destroyed objects
    //loop over shells and check if one of them is destroyed if so remove
    activeShells.erase(std::remove_if(activeShells.begin(), activeShells.end(),
        [](const Shell& s) { return s.remove(); }), activeShells.end());
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

bool legalMove(int move,int player){
    //for now
    return true;
}

void GameManager::displayGameState() const {
    std::cout << "\n=== Game State ===" << std::endl;
    std::cout << "Step: " << gameSteps << std::endl;
    std::cout << "Shells left: " << shellsLeft << std::endl;
    //board.displayBoard();
}