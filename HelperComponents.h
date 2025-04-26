#pragma once

enum class Direction {
    U,          // (0) UP
    UR,         // (1) UP_RIGHT
    R,          // (2) RIGHT
    DR,         // (3) DOWN_RIGHT
    D,          // (4) DOWN
    DL,         // (5) DOWN_LEFT
    L,          // (6) LEFT
    UL          // (7) UP_LEFT
};

struct Position {
    int x, y;
    Position(int x, int y) : x(x), y(y) {} //??
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};
