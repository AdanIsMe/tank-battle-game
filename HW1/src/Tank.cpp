#include "../include/Tank.h"
#include "../include/Direction.h"

Tank::Tank(int playerId, int x, int y, Direction direction) : GameObject(x, y), playerId(playerId), direction(direction)  {

}

std::string Tank::getSymbol() const {
    return std::to_string(playerId);
}

void Tank::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void Tank::setDirection(Direction newDirection) {
    direction = newDirection;
}

void Tank::decreaseShootCooldown() {
    if (shootCooldown > 0) {
        shootCooldown--;
    }
}

void Tank::shoot() {
    if (shellsLeft > 0 && shootCooldown == 0) {
        shellsLeft--;
        shootCooldown = 5;//so we don't count the step were on 
    }
}

void Tank::startBackwardMove() {
    if (backwardMoveStep == 0) {
        backwardMoveStep = 1;
    }
}

void Tank::incraeseWaitTime(){
    if (backwardMoveStep < 5) {
        backwardMoveStep++;}
    
}

void Tank::cancelBackwardMove() {
    backwardMoveStep = 0;
}

bool Tank::isInBackwardMove() const {
    return backwardMoveStep > 0;
}

bool Tank::isTank() const { return true; }
int Tank::getPlayerId() const { return playerId; }
Direction Tank::getDirection() const { return direction; }
int Tank::getShellsLeft() const { return shellsLeft; }
int Tank::getShootCooldown() const { return shootCooldown; }
int Tank::getBackwardMoveStep() const { return backwardMoveStep; }