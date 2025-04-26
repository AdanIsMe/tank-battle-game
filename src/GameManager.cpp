#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility> // for std::pair
#include <vector>

#include "../algorithms/ChaseAlgorithm.h"
#include "../algorithms/DefensiveAlgorithm.h"

//#include "../include/DirectionUtil.h"
#include "../include/Direction.h"
#include "../include/GameBoard.h"
#include "../include/GameManager.h"
#include "../include/Mine.h"
#include "../include/Shell.h"
#include "../include/Tank.h"
#include "../include/Wall.h"


GameManager::GameManager(const std::string& inputFile, const std::string& outputFile) 
    : inputFile(inputFile), outputFile(outputFile) {}

GameManager::~GameManager() {
    if (outStream.is_open()) {
        outStream.close();
    }
}

bool GameManager::initialize() {
    // Open output file
    outStream.open(outputFile);
    if (!outStream.is_open()) {
        std::cerr << "Failed to open output file: " << outputFile << std::endl;
        return false;
    }
    
    // First pass to get dimensions
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open input file: " << inputFile << std::endl;
        return false;
    }
    
    int width = 0, height = 0;
    inFile >> width >> height;
    inFile.close();
    
    // Validate dimensions
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid board dimensions: " << width << "x" << height << std::endl;
        return false;
    }
    
    // Create board with correct dimensions
    board = std::make_unique<GameBoard>(width, height);
    
    // Load game board content
    std::string errors;
    if (!board->loadFromFile(inputFile, errors)) {
        std::cerr << "Failed to load game board: " << errors << std::endl;
        return false;
    }
    
    // Write input errors to file if any
    if (!errors.empty()) {
        std::ofstream errorFile("input_errors.txt");
        if (errorFile.is_open()) {
            errorFile << errors;
            errorFile.close();
        }
    }
    
    // Initialize algorithms
    algorithm1 = std::make_unique<ChaseAlgorithm>();
    algorithm2 = std::make_unique<DefensiveAlgorithm>();
    
    outStream << "Game initialized with:\n";
    outStream << "Board size: " << width << "x" << height << "\n";
    outStream << "Player 1 algorithm: " << algorithm1->getName() << "\n";
    outStream << "Player 2 algorithm: " << algorithm2->getName() << "\n\n";
    
    return true;
}

void GameManager::runGame() {
    outStream << "Starting game...\n";
    
    while (!gameOver && steps < 1000) { // Safety limit
        processStep();
        steps++;
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
        tank->cancelBackwardMove();
    }

    if (tank->isInBackwardMove() && action != "MOVE_FORWARD"){
        //we reached 3rd step 
        if (tank->getBackwardMoveStep() == 3){
            performBackwardMove(tank);
        }
        else{
            writeAction(playerId, "MOVE_BACKWARD(still waiting)", false);
        }
        return;
    }
    
    else if (action == "MOVE_FORWARD") {
        // Cancel any pending backward move
        if (tank->isInBackwardMove()) {
            tank->cancelBackwardMove();
            writeAction(playerId, "MOVE_FORWARD (canceled MOVE_BACKWARD)", false);
            return;
        }
        
        auto [dx, dy] = DirectionUtil::getMovement(dir);
        int nx = x + dx;
        int ny = y + dy;
        wrapPosition(nx, ny);
        
        // Check if move is valid
        bool canMove = true;
        for (const auto& obj : board->getObjectsAt(nx, ny)) {
            if (obj->isCollidable() && obj.get() != tank.get()) {
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

    else if (action == "MOVE_BACKWARD") {
        if (!tank->isInBackwardMove()) {
            // we start waiting
            tank->startBackwardMove();
            writeAction(playerId, "MOVE_BACKWARD (start waiting time)", false);
            return;
        }
        else if (tank->getBackwardMoveStep() >= 3) {
            //1. we are on the third move
            //2. no need to wait after consecutive backward moves
            performBackwardMove(tank);
        }
    }

    else if (action == "ROTATE_LEFT") {
        Direction newDir = DirectionUtil::rotateLeft(dir);
        tank->setDirection(newDir);
        writeAction(playerId, "ROTATE_LEFT to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "ROTATE_RIGHT") {
        Direction newDir = DirectionUtil::rotateRight(dir);
        tank->setDirection(newDir);
        writeAction(playerId, "ROTATE_RIGHT to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "ROTATE_LEFT_QUARTER") {
        Direction newDir = DirectionUtil::rotateLeftQuarter(dir);
        tank->setDirection(newDir);
        writeAction(playerId, "ROTATE_LEFT_QUARTER to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "ROTATE_RIGHT_QUARTER") {
        Direction newDir = DirectionUtil::rotateRightQuarter(dir);
        tank->setDirection(newDir);
        writeAction(playerId, "ROTATE_RIGHT_QUARTER to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "SHOOT") {
        if (tank->getShootCooldown() == 0 && tank->getShellsLeft() > 0) {
            tank->shoot();
            auto [dx, dy] = DirectionUtil::getMovement(dir);
            int sx = x + dx;
            int sy = y + dy;
            wrapPosition(sx, sy);
            
            auto shell = std::make_shared<Shell>(sx, sy, dir, playerId);
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
    if(tank->isInBackwardMove()){
        tank->incraeseWaitTime(); 
    }
    // Decrease shoot cooldown
    tank->decreaseShootCooldown();
}

void GameManager::checkShellCollisions(const std::shared_ptr<Shell>& shell, int x, int y) {
    for (const auto& obj : board->getObjectsAt(x, y)) {
        if (!obj->isCollidable()) continue;

        if (obj->isWall()) {
            handleWallCollision(obj);
        } else if (obj->isShell()) {
            handleShellCollision(shell, obj);
        } else if (obj->isTank()) {
            handleTankCollision(obj,x,y);
        }

        if (shouldDestroyShell(obj)) {
            board->removeShell(shell);
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
                break;
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
          
    // Check if move is valid
    bool canMove = true;
    for (const auto& obj : board->getObjectsAt(nx, ny)) {
        if (obj->isCollidable() && obj.get() != tank.get()) {
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
    // Process all active shells
    for (const auto& shell : board->getShells()) {
        // Move shell 1 step and get its new position
        shell->move();
        auto [x, y] = shell->getPosition();
        wrapPosition(x, y);

        // Check collisions at new position
        checkShellCollisions(shell, x, y);

        // Move shell another step and get its new position
        shell->move();
        auto [x2, y2] = shell->getPosition();
        wrapPosition(x2, y2);
        
        // Check collisions again at the new position
        checkShellCollisions(shell, x2, y2);
        
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

void GameManager::handleShellCollision(const std::shared_ptr<Shell>& shell1, 
    const std::shared_ptr<GameObject>& shell2) {
    board->removeShell(shell1);
    board->removeShell(std::static_pointer_cast<Shell>(shell2));
}


void GameManager::handleTankCollision(const std::shared_ptr<GameObject>& tank,int x,int y) {
    auto tankPtr = std::static_pointer_cast<Tank>(tank);
    outStream << "Shell hit tank " << tankPtr->getPlayerId() 
              << " at (" << x << "," << y << ")\n";
    board->removeTank(tankPtr);
}

bool GameManager::shouldDestroyShell(const std::shared_ptr<GameObject>& obj) {
    return obj->isWall() || obj->isShell() || obj->isTank();
}

const GameBoard& GameManager::getGameBoard() const { return *board; }
