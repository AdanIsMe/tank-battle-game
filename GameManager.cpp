#include "GameManager.h"

GameManager::GameManager(GameBoard& board_param, int x1, int y1, int x2, int y2)
: board(board_param) 
{
    game_steps = 0;
    zero_shells_left = false;
    game_running = true;
    active_shells = std::vector<std::unique_ptr<Shell>>();
    tie_counter = 0;

    initializeGame(x1, y1, x2, y2);
}


void GameManager::initializeGame(int x1,int y1,int x2,int y2) {
    // Initialize players' tanks - not placing them on the board anymore
    player1.tank.setPosition(x1, y1);
    player2.tank.setPosition(x2,y2);
    
    // Other initialization logic
    zero_shells_left = false; //both players shell number is initilized to be 16
}

void GameManager::play() {
    while (game_running) {
        game_steps++;
        
        // Process player actions - each player algorithm should decide next move 
        int move1 = player1.nextMove();
        int move2 = player2.nextMove();

        if(!legalMove(move1, 1)){
            //if the move is ilegal ignore it
            std::cout << "bad step" << std::endl;
        }
        else{
            //if it's a legal move perform it(printing happens in the action managment)
            player1.tank.action(move1);
            //active shells are added in shoot action
        }

        if(!legalMove(move2, 2)){
            std::cout << "bad step" << std::endl;
        }
        else{

            player2.tank.action(move2);
        }

        // Update shells left status
        zero_shells_left = (player1.tank.getNumOfShells() == 0) && (player2.tank.getNumOfShells() == 0);

        updateGameState();//move shell one step at a time
        updateGameState();//check each time if collision ocurred

        checkWinConditions();
        displayGameState();
    }
}

void GameManager::updateGameState() {
    // Update all shell positions
    for (auto& shell : active_shells) {
        shell->updatePosition();
    }
    
    // Check for collisions
    for (auto& shell : active_shells) {
        if (shell.remove) continue;

        // Check collision with static board objects
        GameObject* obj = board.getObjectAt(shell.getX(), shell.getY());
        GameObject* obj2 = board.getObjectAt(player1.tank.getX(), player1.tank.getY());
        GameObject* obj3 = board.getObjectAt(player2.tank.getX(), player2.tank.getY());

        if (obj != nullptr) {
            handleShellCollision(shell, obj);
        }
        if (obj2 != nullptr) {
            handleTankCollision(player1.tank, obj2);
        }
        if (obj3 != nullptr) {
            handleTankCollision(player2.tank, obj3);
        }

        // Check collision with non-static objects(shell/tanks):

        // Check shell collision with tanks:
        if (shell.getX() == player1.tank.getX() && shell.getY() == player1.tank.getY()) {
            //a shell hits player1 tank
            player1.tank.isAlive = false;
            shell.remove = true;
        }
        if (shell.getX() == player2.tank.getX() && shell.getY() == player2.tank.getY()) {
            //a shell hits player2 tank
            player2.tank.isAlive = false;
            shell.remove = true;
        }

        // Check collision with other shells:
        for (auto& other_shell : active_shells) {
            if (&shell == &other_shell || other_shell.remove) continue;
            
            if (shell.getX() == other_shell.getX() && 
                shell.getY() == other_shell.getY()) {
                shell.remove = true;
                other_shell.remove = true;
            }
        }
    }
    
    // Remove destroyed shells
    active_shells.erase(std::remove_if(active_shells.begin(), active_shells.end(),
    [](const std::unique_ptr<Shell>& s) { return s->remove; }), active_shells.end());


    // Check for tank-to-tank collision
    if (player1.tank.getX() == player2.tank.getX() &&
        player1.tank.getY() == player2.tank.getY()) {
        player1.tank.isAlive = false;
        player2.tank.isAlive = false;
    }
}

void GameManager::handleShellCollision(Shell& shell, GameObject* obj) {
    
    //if (Mine* mine = dynamic_cast<Mine*>(obj)) {
    if (dynamic_cast<Mine*>(obj)) {

        //nothing happens- I think
        //no need to remove the shell
        return;
    }

    shell.remove = true;
    
    if (Wall* wall = dynamic_cast<Wall*>(obj)) {
        if (wall->weaken()) {
            board.removeObject(wall->getX(), wall->getY());
        }
    }

    // Tank collisions are now handled directly in updateGameState
}

void GameManager::handleTankCollision(Tank& tank, GameObject* obj) {   
   
    //if (Wall* wall = dynamic_cast<Wall*>(obj)) {
    if (dynamic_cast<Wall*>(obj)) {

        
        //collision shouldn't happen if we encounter a wall while moving tank can't move there 
        std::cout << "we shouldn't get here we did somthing wrong" << std::endl;
        return;
    }

    if (Mine* mine = dynamic_cast<Mine*>(obj)) {
        
        //destroy mine:
        //Just remove it from the board
        board.removeObject(mine->getX(),mine->getY());

        //destroy tank :
        tank.isAlive = false;
        return;
    }

    // shell collisions are now handled directly in updateGameState
 
}

void GameManager::checkWinConditions() {
    bool p1_alive = player1.tank.isAlive;
    bool p2_alive = player2.tank.isAlive;
    
    if ((!p1_alive && p2_alive) || (p1_alive && !p2_alive)) {
        game_running = false;
        std::cout << "Game over! Winner: " << (p1_alive ? "Player 1" : "Player 2") << std::endl;
    }
    else if (!p1_alive && !p2_alive) {
        game_running = false;
        std::cout << "Game over! Draw - no tanks left." << std::endl;
    }
    else if (zero_shells_left) {
        game_running = (tie_counter <= 40);
        tie_counter++;
        if(!game_running) {
            std::cout << "Game over! Draw - no shells left." << std::endl;
        }
    }
}

void GameManager::displayGameState() const {
    std::cout << "\n=== Game State ===" << std::endl;
    std::cout << "Step: " << game_steps << std::endl;
    std::cout << "Player 1 shells: " << player1.tank.getNumOfShells() << std::endl;
    std::cout << "Player 2 shells: " << player2.tank.getNumOfShells() << std::endl;
    // You might want to add tank positions to the display
}

bool GameManager::legalMove(int move, int player) {
    // Implement proper move validation
    // Should check boundaries and collisions with walls
    return true;
}

void GameManager::addShell(std::unique_ptr<Shell> shell) {
    active_shells.push_back(std::move(shell));
}
