#pragma once
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "../../common/PlayerFactory.h"
#include "../../common/TankAlgorithmFactory.h"
#include "../GameSatelliteView/GameSatelliteView.h"
#include "../../../json.hpp"
using json = nlohmann::json;


class GameManager {
public:
    //GameManager(PlayerFactory&& player_factory, TankAlgorithmFactory&& tank_algorithm_factory);
    GameManager(const PlayerFactory& player_factory, const TankAlgorithmFactory& tank_factory);
    ~GameManager();

    void readBoard(const std::string& filename);
    void runGame();
    void writeOutput(const std::string& filename);

    //visual
    
    void setGameLogFile(const std::string &filename);
    void setVisualizationFile(const std::string &filename);
    void writeVisualizationState();
    
    // Direction embedded in GameManager for clarity and encapsulation 
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
        static Direction getDirectionFromDelta(int dx, int dy);
        static std::string getActionToMove(Direction currentDir, int dx, int dy);
        static std::vector<Direction> allDirections();
    };

private:
    // Factories for creating players and tank algorithms
    //std::unique_ptr<PlayerFactory> player_factory_;
    //std::unique_ptr<TankAlgorithmFactory> tank_algorithm_factory_;
    
    // Factories for creating players and tank algorithms
    const PlayerFactory& player_factory_;
    const TankAlgorithmFactory& tank_algorithm_factory_;

    // Game state
    size_t max_steps_;
    size_t num_shells_;
    size_t overall_shells;
    size_t rows_;
    size_t cols_;
    size_t current_step_;
    
    // Game board
    std::vector<std::vector<char>> board_;
    
    //visual
    std::ofstream game_log_stream_;
    std::ofstream visualization_stream_;
    std::string game_log_file_;
    std::string visualization_file_;
    std::vector<std::string> round_actions_log_;
    std::vector<json> current_round_actions_; // Use json for easier writeRoundStateJson compatibility


    // Game objects
    std::vector<std::unique_ptr<Player>> players_;
    std::vector<std::vector<std::unique_ptr<TankAlgorithm>>> tanks_; // Indexed by [player_index-1][tank_index]
    
    // Tank data (authoritative source)
    struct TankData {
        int player_index;
        int tank_index;
        size_t x;
        size_t y;
        Direction direction;
        size_t remaining_shells;
        bool in_backward_move = false;
        bool in_reverse_mode = false;
        int backward_move_counter;
        bool cooldown_wait;
        int cooldown_timer;  // For tracking shooting cooldown
        bool is_alive;
        
        TankData(int p_idx, int t_idx, size_t x, size_t y, Direction dir, size_t shells)
            : player_index(p_idx), tank_index(t_idx), x(x), y(y), direction(dir),
              remaining_shells(shells), cooldown_timer(0), is_alive(true) {}
    };
    
    // Tanks data
    std::vector<TankData> tanks_data_;
    
    // Shell data
    struct ShellData {
        size_t x;
        size_t y;
        Direction direction;
        ShellData(size_t x, size_t y, Direction dir)
            : x(x), y(y), direction(dir){}
    };
    
    // Wall tracking
    struct WallInfo {
        size_t x;
        size_t y;
        int health;  //  health (2 = new, 1 = damaged, 0 = destroyed)        
        WallInfo(size_t x, size_t y,int health)
            : x(x), y(y),health(health) {}
        
        bool operator==(const WallInfo& other) const {
            return x == other.x && y == other.y;
        }
    };

    // Mines tracking
    struct MineInfo {
        size_t x;
        size_t y;       
        MineInfo(size_t x, size_t y)
            : x(x), y(y){}
        bool operator==(const MineInfo& other) const {
            return x == other.x && y == other.y;
        }
    };
    
    
    std::vector<WallInfo> walls_;
    std::vector<MineInfo> mines_;
    std::vector<ShellData> shells_;
    
    // Output file
    std::string output_file_;
    std::ofstream output_stream_;
 
    // Helper methods
    void initializePlayers();
    void initializeTanks();
    void processGameStep();
    bool isGameOver();
    bool isValidCellChar(char c) const;
    bool CanMoveBackward(GameManager::TankData *tank_data);
    void updateTankCounters(GameManager::TankData *tank_data);
    void addShell(size_t x,size_t y,Direction dir);
    void moveShells();

    void writeRoundStateJson(std::ofstream &out, int round_number);


    void checkCollisions();
    void updateBoard();
    void processAction(int player_idx, int tank_idx, ActionRequest action);
    void logAction(int player_idx, int tank_idx, ActionRequest action, bool is_valid);
    void logGameResult(const std::string& result, const std::string& reason);

    void displayBoard() const;

    ///////////////////////////////////////////////////////////////////////////////////
    bool processBackwardMove(TankData* tank_data, int player_idx, int tank_idx, ActionRequest action);
    bool processMoveForward(TankData* tank_data);
    bool processMoveBackward(TankData* tank_data, int player_idx, int tank_idx);
    void processRotation(TankData* tank_data, ActionRequest action);
    bool processShoot(TankData* tank_data);
    ///////////////////////////////////////////////////////////////////////////////////
    ///
    //std::pair<int, int> getDirectionDelta(Direction dir);
    //Direction rotateLeft(Direction dir, int steps);
    //Direction rotateRight(Direction dir, int steps);

};
