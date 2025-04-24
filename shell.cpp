#include "shell.h"

Shell::Shell(int x, int y, Direction direction) {
    x =x_coordinate;
    y = y_coordinate;
    direction = direction;
}

bool Shell::shouldRemove()
{
    return remove;
}

Direction Shell::getDirection() const
{
    return direction;
}

void Shell::updatePosition() {
    int board_height = board.getHeight();
    int board_width = board.getWidth();
    
    //move two steps each game step
    switch(direction) {
        // UP: Decrease row by 2 (x -= 2)
        case Direction::U:
            x_coordinate = (x_coordinate - 2 + 2 * board_height) % board_height;
            break;

        // DOWN: Increase row by 2 (x += 2)
        case Direction::D:
            x_coordinate = (x_coordinate + 2) % board_height;
            break;

        // LEFT: Decrease column by 2 (y -= 2)
        case Direction::L:
            y_coordinate = (y_coordinate - 2 + 2 * board_width) % board_width;
            break;

        // RIGHT: Increase column by 2 (y += 2)
        case Direction::R:
            y_coordinate = (y_coordinate + 2) % board_width;
            break;

        // UP-RIGHT: Decrease row by 2, increase column by 2 (x -= 2, y += 2)
        case Direction::UR:
            x_coordinate = (x_coordinate - 2 + 2 * board_height) % board_height;
            y_coordinate = (y_coordinate + 2) % board_width;
            break;
        
        // UP-LEFT: Decrease row by 2, decrease column by 2 (x -= 2, y -= 2)
        case Direction::UL:
            x_coordinate = (x_coordinate - 2 + 2 * board_height) % board_height;
            y_coordinate = (y_coordinate - 2 + 2 * board_width) % board_width;
            break;

        // DOWN-RIGHT: Increase row by 2, increase column by 2 (x += 2, y += 2)
        case Direction::DR:
            x_coordinate = (x_coordinate + 2) % board_height;
            y_coordinate = (y_coordinate + 2) % board_width;
            break;

        // DOWN-LEFT: Increase row by 2, decrease column by 2 (x += 2, y -= 2)
        case Direction::DL: 
            x_coordinate = (x_coordinate + 2) % board_height;
            y_coordinate = (y_coordinate - 2 + 2 * board_width) % board_width;
            break;  
    }
}