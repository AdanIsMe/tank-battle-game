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
    
    //move one steps (each game step shell have to move two steps - game manager handles that)
    switch(direction) {
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
