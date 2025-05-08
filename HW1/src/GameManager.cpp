#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility> // for std::pair
#include <vector>

#include "../algorithms/DefensiveAlgorithm.h"
#include "../algorithms/OffensiveAlgorithm.h"

#include "../include/Direction.h"
#include "../include/GameBoard.h"
#include "../include/GameManager.h"
#include "../include/Mine.h"
#include "../include/Shell.h"
#include "../include/Tank.h"
#include "../include/Wall.h"

#include "GameManager.h"
#include <iostream>

GameManager::GameManager(const std::string& inputFile, const std::string& outputFile) 
    : inputFile(inputFile), outputFile(outputFile) {}

GameManager::~GameManager() {
    if (outStream.is_open()) {
        outStream.close();
    }
}

bool GameManager::initialize() {
    try {
        std::ifstream inFile(inputFile);
        if (!inFile) {
            std::cerr << "[Error] Could not open input file: " << inputFile << "\n";
            return false;
        }

        int width, height;
        if (!(inFile >> width >> height) || width <= 0 || height <= 0) {
            std::cerr << "[Error] Invalid board dimensions in file\n";
            return false;
        }
        inFile.close();

        board = std::make_unique<GameBoard>(width, height);

        std::string loadErrors;
        if (!board->loadFromFile(inputFile, loadErrors)) {
            return false;  // Do not create input_errors.txt in this case
        }

        // Open output file
        outStream.open(outputFile);
        if (!outStream) {
            std::cerr << "[Error] Could not open output file: " << outputFile << "\n";
            return false;
        }

        // Initialize algorithms
        algorithm1 = std::make_unique<OffensiveAlgorithm>();
        algorithm2 = std::make_unique<DefensiveAlgorithm>();

        // Game init summary
        outStream << "Game initialized with:\n"
                  << "Board size: " << width << "x" << height << "\n"
                  << "Player 1 algorithm: " << algorithm1->getName() << "\n"
                  << "Player 2 algorithm: " << algorithm2->getName() << "\n\n";

        return true;

    } catch (const std::exception& e) {
        std::cerr << "[Error] Initialization exception: " << e.what() << "\n";
        return false;
    }
}

void GameManager::runGame() {
    outStream << "Starting game...\n";
    std::ofstream logFile("game_flow.txt");

    
    while (!gameOver && steps < 1000) { // Safety limit
        std::cout << "\nStep " << steps << ":\n";
        logFile << "Board size: " << board->getWidth() << "x" << board->getHeight() << "\n\n";

        processStep();
        board->appendStepToLog(logFile, steps); 
        steps++;
        std::cout << "board state at the end of the step " << steps << ":\n";
        board->displayBoard();
        std::cout << "__________________________\n";
    }
    
    outStream << "\nGame ended after " << steps << " steps.\n";
    outStream << "Result: " << result << "\n";
}

void GameManager::processStep() {
    outStream << "\nStep " << steps << ":\n";  

    // Get actions from algorithms
    auto tank1 = board->getPlayerTank(1);
    auto tank2 = board->getPlayerTank(2);
    
    if (tank1) {
        std::string action1 = algorithm1->getNextAction(*board, 1);
        processTankAction(tank1, action1);
    }
    
    if (tank2) {
        std::string action2 = algorithm2->getNextAction(*board, 2);
        processTankAction(tank2, action2);
    }
    
    // Process shells
    processShells();
    
    // Check collisions
    checkCollisions();
    
    // Check game end conditions
    checkGameEnd();
}

void GameManager::processTankAction(const std::shared_ptr<Tank>& tank, const std::string& action) {
    int playerId = tank->getPlayerId();
    auto [x, y] = tank->getPosition();
    Direction dir = tank->getDirection();

    if(tank->getBackwardMoveStep() > 3 && action != "MOVE_BACKWARD"){
        //reset consecutive backward moves
        //cancel reverse mode
        tank->cancelBackwardMove();
    }

    if (tank->isInBackwardMove() && tank->getBackwardMoveStep()<= 3  && action != "MOVE_FORWARD"){
        //we reached 3rd step 
        if (tank->getBackwardMoveStep() == 3){
            performBackwardMove(tank);
            
        }
        else if(tank->getBackwardMoveStep() < 3){
            writeAction(playerId,action + " :while waiting for moving backward- can't perform any other move", false);
        }  
        
        // for backward move if we are waiting
        if(tank->getBackwardMoveStep()<=3 && tank->isInBackwardMove() ){
            tank->incraeseWaitTime(); 
        }
        // Decrease shoot cooldown
        tank->decreaseShootCooldown();
        return;
    }
    
    else if (action == "MOVE_FORWARD") {
        // Cancel any pending backward move
        tank->cancelBackwardMove();
        if (tank->isInBackwardMove()) {
            writeAction(playerId, "MOVE_FORWARD (canceled MOVE_BACKWARD)", false);
        }
        else{ 
            auto [dx, dy] = DirectionUtil::getMovement(dir);
            int nx = x + dx;
            int ny = y + dy;
            wrapPosition(nx, ny);
            
            // Check if move is valid( there is a wall)
            bool canMove = true;
            for (const auto& obj : board->getObjectsAt(nx, ny)) {
                if (obj->isWall()){
                    std::cout <<"cant move wall at " << nx <<"," << ny <<std::endl;
                    canMove = false;
                    break;
                }
            }

            if (canMove) {
                tank->setPosition(nx, ny);
                writeAction(playerId, "MOVE_FORWARD", false);
            } else {
                writeAction(playerId, "MOVE_FORWARD", true);
            }
        }
    } 

    else if (action == "MOVE_BACKWARD") {
        //std::cout<< " player "<< playerId << " : backward moves: "<< tank->getBackwardMoveStep() << std::endl;
        if (!tank->isInBackwardMove()) {
            // we start waiting
            tank->startBackwardMove();
            //std::cout<< " player "<< playerId << " : initiate Backward Move" << std::endl;
            writeAction(playerId, "MOVE_BACKWARD (start waiting time)", false);
        }
        else if (tank->getBackwardMoveStep() >= 3) {
            //1. we are on the third move
            //2. no need to wait after consecutive backward moves
            performBackwardMove(tank);
        }
    }

    else if (action == "ROTATE_LEFT") {
        tank->cancelBackwardMove();
        
        Direction newDir = DirectionUtil::rotateLeft(dir);
        tank->setDirection(newDir);
        writeAction(playerId, "ROTATE_LEFT to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "ROTATE_RIGHT") {
        tank->cancelBackwardMove();
        
        Direction newDir = DirectionUtil::rotateRight(dir);
        tank->setDirection(newDir);
        writeAction(playerId, "ROTATE_RIGHT to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "ROTATE_LEFT_QUARTER") {
        tank->cancelBackwardMove();
        
        Direction newDir = DirectionUtil::rotateLeftQuarter(dir);
        tank->setDirection(newDir);
        writeAction(playerId, "ROTATE_LEFT_QUARTER to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "ROTATE_RIGHT_QUARTER") {
        tank->cancelBackwardMove();
        
        Direction newDir = DirectionUtil::rotateRightQuarter(dir);
        tank->setDirection(newDir);
        writeAction(playerId, "ROTATE_RIGHT_QUARTER to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "SHOOT") {
        tank->cancelBackwardMove();
        if (tank->getShootCooldown() == 0 && tank->getShellsLeft() > 0) {
            tank->shoot();
            
            auto shell = std::make_shared<Shell>(x, y, dir, playerId);

            board->addShell(shell);
            writeAction(playerId, "SHOOT", false);
        } else {
            writeAction(playerId, "SHOOT", true);
        }
    }
    else if (action == "NONE") {
        writeAction(playerId, "NONE", false);
    }

    // for backward move if we are waiting
    if(tank->getBackwardMoveStep()<=3 && tank->isInBackwardMove()){
        tank->incraeseWaitTime(); 
    }
    // Decrease shoot cooldown
    tank->decreaseShootCooldown();
}

// is called when a shell moves to a new position
// and checks for collisions with other objects
void GameManager::checkShellCollisions(const std::shared_ptr<Shell>& shell, int x, int y) {
    auto objects = board->getObjectsAt(x, y);

    if (objects.size() <= 1){
        return; // No collision or only the shell itself{
    }

    // Handle multiple object collisions
    bool hasTank = false;
    bool hasMine = false;
    bool hasWall = false;
    bool hasShell = false;
    std::shared_ptr<Tank> tank;
    std::shared_ptr<Wall> wall;
    std::shared_ptr<Mine> mine;

    // First pass to identify what objects we have
    for (const auto& obj : objects) {
        if (!obj->isCollidable()) continue;
        
        if (obj->isTank()) {
            hasTank = true;
            tank = std::static_pointer_cast<Tank>(obj);
        } else if (obj->isMine()) {
            hasMine = true;
            mine = std::static_pointer_cast<Mine>(obj);
        } else if (obj->isWall()) {
            hasWall = true;
            wall = std::static_pointer_cast<Wall>(obj);
        } else if (obj->isShell()) {
            hasShell = true; // should be true
        }
    }

    // Handle 2-object collisions
    if (objects.size() == 2) {

        // shell + tank
        if (hasShell && hasTank) {
            board->removeShell(shell);
            board->removeTank(tank);
            outStream << "Shell and Tank " << tank->getPlayerId() 
                      << " destroyed at (" << x << "," << y << ")\n";
            return;
        }
        // shell + shell
        else if (hasShell && !hasTank && !hasWall && !hasMine) {
            // Find the other shell
            for (const auto& obj : objects) {
                if (obj->isShell() && obj != shell) {

                    // Destroy both shells
                    board->removeShell(shell);
                    board->removeShell(std::static_pointer_cast<Shell>(obj));
                    outStream << "Two shells destroyed each other at (" 
                             << x << "," << y << ")\n";
                    return;
                }
            }
        }
        // shell + wall
        else if (hasShell && hasWall) {
            handleWallCollision(wall);
            board->removeShell(shell);

            outStream << "Shell hit wall at (" << x << "," << y << ")\n";
            return;
        }

    }
    // Handle 3+ object collisions
    else
    {
        // Case 1: shell + mine + tank
        if (hasMine && hasTank) {
            // Destroy all objects at this cell
            for (const auto& obj : objects) {
                if (obj->isTank()) {
                    board->removeTank(std::static_pointer_cast<Tank>(obj));
                } else if (obj->isShell()) {
                    board->removeShell(std::static_pointer_cast<Shell>(obj));
                } else if (obj->isMine()) {
                    board->removeMine(std::static_pointer_cast<Mine>(obj));
                }
            }
            outStream << "Multiple objects including tank and mine destroyed at (" 
                     << x << "," << y << ")\n";
            return;
        }
       
        // Case 3: shell + mine
        else if (hasMine && !hasTank) {
            // 3+ objects, including mine therefore no wall(because they dont move and initially in different cells)
            // and no tank. conclusion: there's another shell

            for (const auto& obj : objects) {
                //if (obj == shell) continue;
                if (!obj->isCollidable()) continue;
 
                if (obj->isShell()) {
                    board->removeShell(std::static_pointer_cast<Shell>(obj));
                }
            }
            //board->removeShell(shell);
            outStream << "Shells hit objects (mine unaffected) at (" 
                     << x << "," << y << ")\n";

            return;
        }
        // Case 4: shell + shell + tank
        else if (hasTank && !hasMine) {
            // 3+ objects, including tank therefore no wall(because they dont move and initially in different cells)
            // and no mine. conclusion: there's another shell

            for (const auto& obj : objects) {
                //if (obj == shell) continue;
                //if (!obj->isCollidable()) continue;
 
                if (obj->isShell()) {
                    board->removeShell(std::static_pointer_cast<Shell>(obj));
                }

                if (obj->isTank()) {
                    board->removeTank(std::static_pointer_cast<Tank>(obj));
                }
                
            }
            //board->removeShell(shell);
            outStream << "Shells hit tank at (" << x << "," << y << ")\n";
            return;
        }
    }
}

void GameManager::checkCollisions() {
    // Check tank-mine collisions
    for (const auto& tank : board->getTanks()) {
        auto [x, y] = tank->getPosition();
        for (const auto& mine : board->getMines()) {
            auto [mx, my] = mine->getPosition();
            if (x == mx && y == my) {
                board->removeTank(tank);
                board->removeMine(mine);
                outStream << "Tank " << tank->getPlayerId() << " hit a mine at (" << x << "," << y << ")\n";
                return;
            }
        }
    }
    
    // Check tank-tank collisions
    auto tanks = board->getTanks();
    if (tanks.size() >= 2) {
        auto tank1 = tanks[0];
        auto tank2 = tanks[1];
        auto [x1, y1] = tank1->getPosition();
        auto [x2, y2] = tank2->getPosition();
        
        if (x1 == x2 && y1 == y2) {
            board->removeTank(tank1);
            board->removeTank(tank2);
            outStream << "Tanks collided at (" << x1 << "," << y1 << ")\n";
        }
    }
}

void GameManager::performBackwardMove(const std::shared_ptr<Tank>& tank){
    // Perform the backward move 
    int playerId = tank->getPlayerId();
    auto [x, y] = tank->getPosition();
    Direction dir = tank->getDirection();

    auto [dx, dy] = DirectionUtil::getMovement(dir);
    int nx = x - dx;
    int ny = y - dy;
    wrapPosition(nx, ny);
          
    // Check if move is valid (thers is a wall)
    bool canMove = true;
    for (const auto& obj : board->getObjectsAt(nx, ny)) {
        if (obj->isWall()) {
            canMove = false;
            break;
        }
    }
          
    if (canMove) {
        tank->setPosition(nx, ny);
        writeAction(playerId, "MOVE_BACKWARD", false);
    } else {
        writeAction(playerId, "MOVE_BACKWARD", true);
    }
}


void GameManager::processShells() {
    // First cell move() for all shells
    for (const auto& shell : board->getShells()) {
        // Move shell 1 step and get its new position
        move(*shell);            
    }

    auto shellsCopy = board->getShells(); // copy by value
    for (const auto& shell : shellsCopy) {
        auto [x, y] = shell->getPosition();
        checkShellCollisions(shell, x, y);
    }

    // Second cell move() for all shells
    for (const auto& shell : board->getShells()) {
        // Move shell 1 step and get its new position
        move(*shell);
    }

    shellsCopy = board->getShells(); // copy by value
    
    for (const auto& shell : shellsCopy) {
        auto [x, y] = shell->getPosition();
        checkShellCollisions(shell, x, y);
    }

}


void GameManager::checkGameEnd() {
    auto tanks = board->getTanks();
    bool player1Alive = false;
    bool player2Alive = false;
    int zeroShellsLeft = 0;
    
    for (const auto& tank : tanks) {
        if (tank->getPlayerId() == 1) player1Alive = true;
        if (tank->getPlayerId() == 2) player2Alive = true;
        zeroShellsLeft += tank->getShellsLeft();
    }
    
    if (!player1Alive && !player2Alive) {
        gameOver = true;
        result = "Tie - both tanks destroyed";
    } else if (!player1Alive) {
        gameOver = true;
        result = "Player 2 wins - Player 1 tank destroyed";
    } else if (!player2Alive) {
        gameOver = true;
        result = "Player 1 wins - Player 2 tank destroyed";
    } else if (zeroShellsLeft == 0) {
        noShellsSteps++;
        if (noShellsSteps >= 40) {
            gameOver = true;
            result = "Tie - no shells left and 40 steps passed";
        }
    } else {
        noShellsSteps = 0;
    }
}

void GameManager::writeAction(int playerId, const std::string& action, bool isBad) {
    outStream << "Player " << playerId << ": " << action;
    if (isBad) outStream << " [BAD STEP]";
    outStream << "\n";
}

void GameManager::wrapPosition(int& x, int& y) {
    board->wrapCoordinates(x, y);
}

/////// helper functions: ///////

void GameManager::handleWallCollision(const std::shared_ptr<GameObject>& wall) {
    auto wallPtr = std::static_pointer_cast<Wall>(wall);
    wallPtr->hit();
    if (wallPtr->isDestroyed()) {
        board->removeWall(wallPtr);
    }
}

const GameBoard& GameManager::getGameBoard() const { return *board; }

void GameManager::move(Shell& shell) {
    Direction dir = shell.getDirection();
    auto [dx, dy] = DirectionUtil::getMovement(dir);
    auto [x, y] = shell.getPosition();
    x = x + dx; // Shells move 2 units per step
    y = y + dy; 
    wrapPosition(x, y);
    shell.setPosition(x,y);
}
