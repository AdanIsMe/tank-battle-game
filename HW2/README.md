# 🧨 Tank Battle Simulator and Visualizer

## 👤 Contributors
- **Shaimaa Hoji** – 211961057
- **Adan Assi** – 322719139

---

## 🧠 Overview

This project implements a complete 2-player tank battle simulation system, including both a **game engine (C++)** and a **visualizer (Python)**.

Each tank player is controlled by an AI algorithm:
- Player 1 uses an **aggressive offense-oriented strategy**
- Player 2 employs a **defensive evasion strategy**

The simulation runs turn-by-turn on a toroidal grid-based map with walls, mines, and other hazards. The game records each state to log files, and the Python visualizer can animate the entire play-by-play.

---

## 🧾 Input Format

### Board File (e.g., `input.txt`)
The simulation accepts a plain text file describing the initial game board:

```
<width> <height>
<row 1>
<row 2>
...
<row H>
```

### Symbols

| Symbol | Meaning               |
|--------|------------------------|
| `1`    | Player 1 Tank          |
| `2`    | Player 2 Tank          |
| `#`    | Wall (takes 2 hits)    |
| `@`    | Mine (instant destroy) |
| ` `    | Empty space            |

#### Example:
```
7 7
1     @
  ###  
     2
```

Warnings such as duplicate tanks or unknown characters are logged to `input_errors.txt`.

---

## 🕹️ Game Logic

- Each tank can **move**, **rotate**, or **shoot**
- **Backward moves** require 3 steps (wind-up)
- **Shells** move 2 steps per turn
- **Walls** require 2 hits to be destroyed
- **Mines** destroy anything stepping on them
- Game ends when:
  - One or both tanks are destroyed
  - 40 steps pass with no shells remaining

---

## 🧠 AI Player Strategies

### 🚀 OffensiveAlgorithm (Player 1)
- Prioritizes **chasing and shooting**
- Uses **BFS pathfinding** to close in
- Shoots if the enemy is in line-of-sight and cooldown allows
- Avoids hazards when a shell is approaching

### 🛡️ DefensiveAlgorithm (Player 2)
- Prioritizes **survival**
- Avoids mines and shells
- Only shoots when it is safe
- Seeks safe adjacent tiles and hiding directions

---

## 📁 File Structure

| File / Folder                  | Description |
|--------------------------------|-------------|
| `main.cpp`                     | Entry point of the simulation |
| `GameManager.*`                | Runs the game loop, checks win conditions |
| `GameBoard.*`                  | Represents and manages the grid and objects |
| `Tank.*`, `Wall.*`, `Mine.*`, `Shell.*` | Classes for in-game entities |
| `Direction.*`                  | Handles directional logic and rotation |
| `TankAlgorithm.*`              | Abstract base for AI logic |
| `OffensiveAlgorithm.*`         | Player 1's AI implementation |
| `DefensiveAlgorithm.*`         | Player 2's AI implementation |
| `Makefile`                     | Optional build automation |
| `input.txt`, `test_*.txt`      | Test boards |
| `game_output.txt`              | Turn-by-turn action log |
| `game_flow.txt`                | Visual step log (used by Python visualizer) |
| `tank_game_visualizer.py`      | Python script to animate `game_flow.txt` |

---

## 📤 Output Files

- `game_output.txt`: Actions per player per turn
- `game_flow.txt`: Full board state at each turn for visual playback
- `input_errors.txt`: Recoverable warnings during input parsing

---

## 🧪 Test Boards

Included sample inputs:
- `test_all_mines.txt`: Tanks surrounded by mines
- `test_all_walls.txt`: Enclosed boards
- `test_1_surrounded_by_walls.txt`, `test_2_surrounded_by_mines.txt`
- `test_empty_board.txt`: Open field testing

These are great for debugging pathfinding, survival logic, and edge cases.

---

## ⚙️ Compilation & Running (C++)

If using the provided object files or Makefile:
```bash
make
./main input.txt
```

This runs the full simulation, generating the output files for analysis or visualization.

---

## ✅ Summary

This project combines AI decision-making, grid simulation, and visual playback to provide a robust tank battle platform. It supports:
- Modular AI plug-in logic
- Detailed turn logging
- Graphical replay system
- Multiple test configurations

Perfect for testing game AI strategies and simulating battle scenarios.

