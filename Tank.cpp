#include "Tank.h"
#include <iostream>



// Constructor implementation
Tank::Tank() : num_of_shells(16) {
}

//this function ensures one game step per action
void Tank::action(int command) {
    
    //the tank cannot shoot again for 4 steps
    if(shoot_cnt > 0 && shoot_cnt < 4){
        shoot_cnt++;
    }
    else{
        shoot_cnt = 0;
    }

    // If we're in waiting state (first 2 steps after backward request)
    if (waited > 0 && waited < 3) {
        waited++;
        // Forward cancels one pending backward
        if(command == 1){
            std::cout << "Forward canceled one pending backward" << std::endl;
            waited = 0;
            return;
        }
        // Additional backward requests during waiting are ignored
        // Other commands are also ignored during waiting
        return;
    }
    // If we just completed waiting (3rd step)
    else if (waited == 3) {
        if(command == 1){
            std::cout << "Forward canceled one pending backward" << std::endl;
            waited = 0;
        }
        // Execute the pending backward move
        else{
            moveBackward();
            //to check if we get another backward command immediately after executing
            waited = 4;
        }

        return;
    }
    //check for consecutive backward moves
    else if (waited == 4 && command != 2){
        // the tank stopped or performed an action other than moving backward we reset waiting time
        waited = 0;   
    }

    // Handle normal commands when not waiting
    switch (command) {
        case 1: // Move forward
            moveForward();
            break;
            
        case 2: // Move backward
            if(waited == 4){
                //consecutive backward moves takes only one game step
                moveBackward();//takes only one game step
                return;
            }
            else{
                //New backward request starts waiting period
                waited = 1; // Start counting (1, 2, then execute on 3)
            }
            break;
            
        case 3: // Shoot
            if (num_of_shells > 0) {
                if(shoot_cnt == 0){
                    shoot();
                    shoot_cnt = 1; //start counting steps
                    num_of_shells--;
                }
            }
            else{
                //std::cout << "can't shoot yet" << std::endl;
                std::cout << "bad step" << std::endl;
            }
            break;
            
        default:
            std::cout << "bad step" << std::endl;
    }
}

void Tank::shoot(){
    //initial idea: create Artillery shell object with cooardinate and direction
    //and add it to the game board until it explodes  
    //maybe the game manager should keep track of it
    //keep a list of shells that were launched and update it on the board 
}

void Tank::moveForward() {
    Direction curr = cannon.getCurrentDirection(); // Get the current direction of the cannon
    int board_height = board.getHeight();
    int board_width = board.getWidth();

    switch(curr) {
        // UP: Decrease row (x--) since rows grow downward
        case Direction::U:
            x_coordinate = (x_coordinate - 1 + board_height) % board_height;
            break;

        // DOWN: Increase row (x++)
        case Direction::D:
            x_coordinate = (x_coordinate + 1) % board_height;
            break;

        // LEFT: Decrease column (y--)
        case Direction::L:
            y_coordinate = (y_coordinate - 1 + board_width) % board_width;
            break;

        // RIGHT: Increase column (y++)
        case Direction::R:
            y_coordinate = (y_coordinate + 1) % board_width;
            break;

        // UP-RIGHT: Decrease row, increase column (x--, y++)
        case Direction::UR:
            x_coordinate = (x_coordinate - 1 + board_height) % board_height;
            y_coordinate = (y_coordinate + 1) % board_width;
            break;
        
        // UP-LEFT: (x--, y--)
        case Direction::UL:
            x_coordinate = (x_coordinate - 1 + board_height) % board_height;
            y_coordinate = (y_coordinate - 1 + board_width) % board_width;
            break;

        // DOWN-RIGHT: (x++, y++)
        case Direction::DR:
            x_coordinate = (x_coordinate + 1) % board_height;
            y_coordinate = (y_coordinate + 1) % board_width;
            break;

        // DOWN-LEFT: (x++, y--)
        case Direction::DL: 
            x_coordinate = (x_coordinate + 1) % board_height;
            y_coordinate = (y_coordinate - 1 + board_width) % board_width;
            break;  
    }
}

void Tank::moveBackward() {
    Direction curr = cannon.getCurrentDirection(); // Get the current direction of the cannon
    int board_height = board.getHeight();
    int board_width = board.getWidth();

    switch(curr) {
        // UP (forward) becomes DOWN (backward)
        case Direction::U:
            x_coordinate = (x_coordinate + 1) % board_height;
            break;

        // DOWN (forward) becomes UP (backward)
        case Direction::D:
            x_coordinate = (x_coordinate - 1 + board_height) % board_height;
            break;

        // LEFT (forward) becomes RIGHT (backward)
        case Direction::L:
            y_coordinate = (y_coordinate + 1) % board_width;
            break;

        // RIGHT (forward) becomes LEFT (backward)
        case Direction::R:
            y_coordinate = (y_coordinate - 1 + board_width) % board_width;
            break;

        // UP-RIGHT (forward) becomes DOWN-LEFT (backward)
        case Direction::UR:
            x_coordinate = (x_coordinate + 1) % board_height;
            y_coordinate = (y_coordinate - 1 + board_width) % board_width;
            break;
        
        // UP-LEFT (forward) becomes DOWN-RIGHT (backward)
        case Direction::UL:
            x_coordinate = (x_coordinate + 1) % board_height;
            y_coordinate = (y_coordinate + 1) % board_width;
            break;

        // DOWN-RIGHT (forward) becomes UP-LEFT (backward)
        case Direction::DR:
            x_coordinate = (x_coordinate - 1 + board_height) % board_height;
            y_coordinate = (y_coordinate - 1 + board_width) % board_width;
            break;

        // DOWN-LEFT (forward) becomes UP-RIGHT (backward)
        case Direction::DL: 
            x_coordinate = (x_coordinate - 1 + board_height) % board_height;
            y_coordinate = (y_coordinate + 1) % board_width;
            break;  
    }
}
