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

GameManager::GameManager(const std::string& input_file, const std::string& output_file) 
    : input_file(input_file), output_file(output_file) {}

GameManager::~GameManager() {
    if (out_stream.is_open()) {
        out_stream.close();
    }
}

bool GameManager::initialize() {
    try {
        std::ifstream in_file(input_file);
        if (!in_file) {
            std::cerr << "[Error] Could not open input file: " << input_file << "\n";
            return false;
        }

        int width, height;
        if (!(in_file >> width >> height) || width <= 0 || height <= 0) {
            std::cerr << "[Error] Invalid board dimensions in file\n";
            return false;
        }
        in_file.close();

        board = std::make_unique<GameBoard>(width, height);

        std::string load_errors;
        if (!board->loadFromFile(input_file, load_errors)) {
            return false;  // Do not create input_errors.txt in this case
        }

        // Open output file
        out_stream.open(output_file);
        if (!out_stream) {
            std::cerr << "[Error] Could not open output file: " << output_file << "\n";
            return false;
        }

        // Initialize algorithms
        algorithm1 = std::make_unique<OffensiveAlgorithm>();
        algorithm2 = std::make_unique<DefensiveAlgorithm>();

        // Game init summary
        out_stream << "Game initialized with:\n"
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
    out_stream << "Starting game...\n";
    std::ofstream log_file("game_flow.txt");

    
    while (!game_over && steps < 1000) { // Safety limit
        std::cout << "\nStep " << steps << ":\n";
        log_file << "Board size: " << board->getWidth() << "x" << board->getHeight() << "\n\n";

        processStep();
        board->appendStepToLog(log_file, steps); 
        steps++;
        std::cout << "board state at the end of the step " << steps << ":\n";
        board->displayBoard();
        std::cout << "__________________________\n";
    }
    
    out_stream << "\nGame ended after " << steps << " steps.\n";
    out_stream << "Result: " << result << "\n";
}

void GameManager::processStep() {
    out_stream << "\nStep " << steps << ":\n";  

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
    int player_id = tank->getPlayerId();
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
            writeAction(player_id, action + " :while waiting for moving backward- can't perform any other move", false);
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
            writeAction(player_id, "MOVE_FORWARD (canceled MOVE_BACKWARD)", false);
        }
        else{ 
            auto [dx, dy] = DirectionUtil::getMovement(dir);
            int nx = x + dx;
            int ny = y + dy;
            wrapPosition(nx, ny);
            
            // Check if move is valid( there is a wall)
            bool can_move = true;
            for (const auto& obj : board->getObjectsAt(nx, ny)) {
                if (obj->isWall()){
                    std::cout <<"cant move wall at " << nx <<"," << ny <<std::endl;
                    can_move = false;
                    break;
                }
            }

            if (can_move) {
                tank->setPosition(nx, ny);
                writeAction(player_id, "MOVE_FORWARD", false);
            } else {
                writeAction(player_id, "MOVE_FORWARD", true);
            }
        }
    } 

    else if (action == "MOVE_BACKWARD") {
        //std::cout<< " player "<< playerId << " : backward moves: "<< tank->getBackwardMoveStep() << std::endl;
        if (!tank->isInBackwardMove()) {
            // we start waiting
            tank->startBackwardMove();
            //std::cout<< " player "<< playerId << " : initiate Backward Move" << std::endl;
            writeAction(player_id, "MOVE_BACKWARD (start waiting time)", false);
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
        writeAction(player_id, "ROTATE_LEFT to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "ROTATE_RIGHT") {
        tank->cancelBackwardMove();
        
        Direction newDir = DirectionUtil::rotateRight(dir);
        tank->setDirection(newDir);
        writeAction(player_id, "ROTATE_RIGHT to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "ROTATE_LEFT_QUARTER") {
        tank->cancelBackwardMove();
        
        Direction newDir = DirectionUtil::rotateLeftQuarter(dir);
        tank->setDirection(newDir);
        writeAction(player_id, "ROTATE_LEFT_QUARTER to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "ROTATE_RIGHT_QUARTER") {
        tank->cancelBackwardMove();
        
        Direction newDir = DirectionUtil::rotateRightQuarter(dir);
        tank->setDirection(newDir);
        writeAction(player_id, "ROTATE_RIGHT_QUARTER to " + DirectionUtil::toString(newDir), false);
    }
    else if (action == "SHOOT") {
        tank->cancelBackwardMove();
        if (tank->getShootCooldown() == 0 && tank->getShellsLeft() > 0) {
            tank->shoot();
            
            auto shell = std::make_shared<Shell>(x, y, dir, player_id);

            board->addShell(shell);
            writeAction(player_id, "SHOOT", false);
        } else {
            writeAction(player_id, "SHOOT", true);
        }
    }
    else if (action == "NONE") {
        writeAction(player_id, "NONE", false);
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
    bool has_tank = false;
    bool has_mine = false;
    bool has_wall = false;
    bool has_shell = false;
    std::shared_ptr<Tank> tank;
    std::shared_ptr<Wall> wall;
    std::shared_ptr<Mine> mine;

    // First pass to identify what objects we have
    for (const auto& obj : objects) {
        if (!obj->isCollidable()) continue;
        
        if (obj->isTank()) {
            has_tank = true;
            tank = std::static_pointer_cast<Tank>(obj);
        } else if (obj->isMine()) {
            has_mine = true;
            mine = std::static_pointer_cast<Mine>(obj);
        } else if (obj->isWall()) {
            has_wall = true;
            wall = std::static_pointer_cast<Wall>(obj);
        } else if (obj->isShell()) {
            has_shell = true; // should be true
        }
    }

    // Handle 2-object collisions
    if (objects.size() == 2) {

        // shell + tank
        if (has_shell && has_tank) {
            board->removeShell(shell);
            board->removeTank(tank);
            out_stream << "Shell and Tank " << tank->getPlayerId() 
                      << " destroyed at (" << x << "," << y << ")\n";
            return;
        }
        // shell + shell
        else if (has_shell && !has_tank && !has_wall && !has_mine) {
            // Find the other shell
            for (const auto& obj : objects) {
                if (obj->isShell() && obj != shell) {

                    // Destroy both shells
                    board->removeShell(shell);
                    board->removeShell(std::static_pointer_cast<Shell>(obj));
                    out_stream << "Two shells destroyed each other at (" 
                             << x << "," << y << ")\n";
                    return;
                }
            }
        }
        // shell + wall
        else if (has_shell && has_wall) {
            handleWallCollision(wall);
            board->removeShell(shell);

            out_stream << "Shell hit wall at (" << x << "," << y << ")\n";
            return;
        }

    }
    // Handle 3+ object collisions
    else
    {
        // Case 1: shell + mine + tank
        if (has_mine && has_tank) {
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
            out_stream << "Multiple objects including tank and mine destroyed at (" 
                     << x << "," << y << ")\n";
            return;
        }
       
        // Case 3: shell + mine
        else if (has_mine && !has_tank) {
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
            out_stream << "Shells hit objects (mine unaffected) at (" 
                     << x << "," << y << ")\n";

            return;
        }
        // Case 4: shell + shell + tank
        else if (has_tank && !has_mine) {
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
            out_stream << "Shells hit tank at (" << x << "," << y << ")\n";
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
                out_stream << "Tank " << tank->getPlayerId() << " hit a mine at (" << x << "," << y << ")\n";
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
            out_stream << "Tanks collided at (" << x1 << "," << y1 << ")\n";
        }
    }
}

void GameManager::performBackwardMove(const std::shared_ptr<Tank>& tank){
    // Perform the backward move 
    int player_id = tank->getPlayerId();
    auto [x, y] = tank->getPosition();
    Direction dir = tank->getDirection();

    auto [dx, dy] = DirectionUtil::getMovement(dir);
    int nx = x - dx;
    int ny = y - dy;
    wrapPosition(nx, ny);
          
    // Check if move is valid (thers is a wall)
    bool can_move = true;
    for (const auto& obj : board->getObjectsAt(nx, ny)) {
        if (obj->isWall()) {
            can_move = false;
            break;
        }
    }
          
    if (can_move) {
        tank->setPosition(nx, ny);
        writeAction(player_id, "MOVE_BACKWARD", false);
    } else {
        writeAction(player_id, "MOVE_BACKWARD", true);
    }
}


void GameManager::processShells() {
    // First cell move() for all shells
    for (const auto& shell : board->getShells()) {
        // Move shell 1 step and get its new position
        move(*shell);            
    }

    auto shells_copy = board->getShells(); // copy by value
    for (const auto& shell : shells_copy) {
        auto [x, y] = shell->getPosition();
        checkShellCollisions(shell, x, y);
    }

    // Second cell move() for all shells
    for (const auto& shell : board->getShells()) {
        // Move shell 1 step and get its new position
        move(*shell);
    }

    shells_copy = board->getShells(); // copy by value
    
    for (const auto& shell : shells_copy) {
        auto [x, y] = shell->getPosition();
        checkShellCollisions(shell, x, y);
    }
}


void GameManager::checkGameEnd() {
    auto tanks = board->getTanks();
    bool player1_alive = false;
    bool player2_alive = false;
    int zero_shells_left = 0;
    
    for (const auto& tank : tanks) {
        if (tank->getPlayerId() == 1) player1_alive = true;
        if (tank->getPlayerId() == 2) player2_alive = true;
        zero_shells_left += tank->getShellsLeft();
    }
    
    if (!player1_alive && !player2_alive) {
        game_over = true;
        result = "Tie - both tanks destroyed";
    } else if (!player1_alive) {
        game_over = true;
        result = "Player 2 wins - Player 1 tank destroyed";
    } else if (!player2_alive) {
        game_over = true;
        result = "Player 1 wins - Player 2 tank destroyed";
    } else if (zero_shells_left == 0) {
        no_shells_steps++;
        if (no_shells_steps >= 40) {
            game_over = true;
            result = "Tie - no shells left and 40 steps passed";
        }
    } else {
        no_shells_steps = 0;
    }
}

void GameManager::writeAction(int player_id, const std::string& action, bool is_bad) {
    out_stream << "Player " << player_id << ": " << action;
    if (is_bad) out_stream << " [BAD STEP]";
    out_stream << "\n";
}

void GameManager::wrapPosition(int& x, int& y) {
    board->wrapCoordinates(x, y);
}

/////// helper functions: ///////

void GameManager::handleWallCollision(const std::shared_ptr<GameObject>& wall) {
    auto wall_ptr = std::static_pointer_cast<Wall>(wall);
    wall_ptr->hit();
    if (wall_ptr->isDestroyed()) {
        board->removeWall(wall_ptr);
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
