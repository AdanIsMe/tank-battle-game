# Tank Battle Game Implementation

This directory contains the complete implementation of the tank battle game for Assignment 2, following the provided API requirements. The implementation includes player factories, player classes, tank algorithms, battle info classes, and the game manager.

## File Structure

### Factory Classes
- `MyPlayerFactory.h` and `MyPlayerFactory.cpp`: Factory for creating player objects
- `MyTankAlgorithmFactory.h` and `MyTankAlgorithmFactory.cpp`: Factory for creating tank algorithm objects

### Player Classes
- `MyPlayer.h` and `MyPlayer.cpp`: Base player class with common functionality
- `OffensivePlayer.h` and `OffensivePlayer.cpp`: Offensive player implementation
- `DefensivePlayer.h` and `DefensivePlayer.cpp`: Defensive player implementation

### Tank Algorithm Classes
- `MyTankAlgorithm.h` and `MyTankAlgorithm.cpp`: Base tank algorithm class
- `OffensiveTankAlgorithm.h` and `OffensiveTankAlgorithm.cpp`: Offensive tank algorithm
- `DefensiveTankAlgorithm.h` and `DefensiveTankAlgorithm.cpp`: Defensive tank algorithm

### Battle Info Classes
- `MyBattleInfo.h` and `MyBattleInfo.cpp`: Base battle info class
- `OffensiveBattleInfo.h` and `OffensiveBattleInfo.cpp`: Offensive battle info
- `DefensiveBattleInfo.h` and `DefensiveBattleInfo.cpp`: Defensive battle info

### Satellite View
- `GameSatelliteView.h` and `GameSatelliteView.cpp`: Implementation of the satellite view

### Game Manager
- `GameManager.h` and `GameManager.cpp`: Main game manager that coordinates all components

### Main Program
- `main.cpp`: Entry point for the program

## Implementation Details

### Player Strategy

The implementation includes two distinct player strategies:

1. **Offensive Player**: Focuses on finding and attacking enemy tanks. It calculates the closest enemy tank for each of its tanks and creates attack plans to efficiently target and destroy them.

2. **Defensive Player**: Prioritizes survival and protection. It assesses danger levels, creates escape routes when threatened, and coordinates tanks to defend allies in danger.

### Tank Algorithms

The tank algorithms implement the decision-making logic for individual tanks:

1. **Offensive Tank Algorithm**: Makes decisions to pursue and attack enemy tanks, with logic for shooting when in range and navigating toward targets.

2. **Defensive Tank Algorithm**: Makes decisions to evade danger, defend allies, and find safe positions on the board.

### Battle Info Communication

The battle info classes facilitate communication between players and tanks:

1. **Offensive Battle Info**: Carries target information, path data, and threat assessments.

2. **Defensive Battle Info**: Carries danger assessments, escape routes, and defensive coordination data.

### Game Management

The GameManager handles:
- Reading the game board from input files
- Managing the game loop
- Processing tank actions
- Handling collisions and game state updates
- Writing game results to output files

## Building and Running

To build the project, compile all the .cpp files together. For example:

```bash
g++ -std=c++17 -o tanks_game main.cpp GameManager.cpp MyPlayerFactory.cpp MyTankAlgorithmFactory.cpp MyPlayer.cpp OffensivePlayer.cpp DefensivePlayer.cpp MyTankAlgorithm.cpp OffensiveTankAlgorithm.cpp DefensiveTankAlgorithm.cpp MyBattleInfo.cpp OffensiveBattleInfo.cpp DefensiveBattleInfo.cpp GameSatelliteView.cpp
```

To run the program:

```bash
./tanks_game <game_board_input_file>
```

The program will create an output file named `game_output.txt` with the game results.

## Notes

- The implementation strictly follows the API requirements provided in the assignment.
- The player and tank algorithm classes are designed to be extensible for future enhancements.
- The code includes proper error handling and validation to ensure robustness.
