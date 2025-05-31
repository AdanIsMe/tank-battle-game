#ifndef ACTION_REQUEST_H
#define ACTION_REQUEST_H

enum class ActionRequest { 
    MoveForward, MoveBackward,
    RotateLeft90, RotateRight90, RotateLeft45, RotateRight45,
    Shoot, GetBattleInfo, DoNothing
};


// Note:
// All the actions in the enum work exactly as in exercise 1
// with a new action: “GetBattleInfo” which will be explained below
#endif // ACTION_REQUEST_H
