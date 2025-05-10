#include "../include/Tank.h"
#include "../include/Direction.h"

Tank::Tank(int player_id, int x, int y, Direction direction) : GameObject(x, y), player_id(player_id), direction(direction)  {

}

std::string Tank::getSymbol() const {
    return std::to_string(player_id);
}

void Tank::setPosition(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

void Tank::setDirection(Direction new_direction) {
    direction = new_direction;
}

void Tank::decreaseShootCooldown() {
    if (shoot_cooldown > 0) {
        shoot_cooldown--;
    }
}

void Tank::shoot() {
    if (shells_left > 0 && shoot_cooldown == 0) {
        shells_left--;
        shoot_cooldown = 5;//so we don't count the step were on 
    }
}

void Tank::startBackwardMove() {
    if (backward_step == 0) {
        backward_step = 1;
    }
}

void Tank::incraeseWaitTime(){
    if (backward_step < 5) {
        backward_step++;}
    
}

void Tank::cancelBackwardMove() {
    backward_step = 0;
}

bool Tank::isInBackwardMove() const {
    return backward_step > 0;
}

bool Tank::isTank() const { return true; }
int Tank::getPlayerId() const { return player_id; }
Direction Tank::getDirection() const { return direction; }
int Tank::getShellsLeft() const { return shells_left; }
int Tank::getShootCooldown() const { return shoot_cooldown; }
int Tank::getBackwardMoveStep() const { return backward_step; }