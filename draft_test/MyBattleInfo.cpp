#include "MyBattleInfo.h"

MyBattleInfo::MyBattleInfo()
    : current_x_(0),
      current_y_(0),
      current_direction_(Direction::NONE),
      remaining_shells_(0),
      cooldown_timer_(0),
      board_width_(100),
      board_height_(100),
      current_step_(0),
      max_steps_(5000) {
}

void MyBattleInfo::setCurrentPosition(size_t x, size_t y) {
    current_x_ = x;
    current_y_ = y;
}

void MyBattleInfo::setCurrentDirection(Direction direction) {
    current_direction_ = direction;
}

void MyBattleInfo::setRemainingShells(size_t shells) {
    remaining_shells_ = shells;
}

void MyBattleInfo::setCooldownTimer(int timer) {
    cooldown_timer_ = timer;
}

void MyBattleInfo::setBoardDimensions(size_t width, size_t height) {
    board_width_ = width;
    board_height_ = height;
}

void MyBattleInfo::setGameState(size_t current_step, size_t max_steps) {
    current_step_ = current_step;
    max_steps_ = max_steps;
}
