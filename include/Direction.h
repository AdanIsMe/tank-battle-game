#pragma once
#include <string>
#include <utility>

enum class Direction {
    UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
};

class DirectionUtil {
public:
    static std::pair<int, int> getMovement(Direction dir);
    static Direction rotateLeft(Direction dir);
    static Direction rotateRight(Direction dir);
    static Direction rotateLeftQuarter(Direction dir);
    static Direction rotateRightQuarter(Direction dir);
    static std::string toString(Direction dir);
    static Direction fromString(const std::string& str);
};