#pragma once
#include "GameBoard.h"
#include "TankAlgorithm.h"
#include <memory>
#include <fstream>
#include <vector>

class GameManager {
public:
    GameManager(const std::string& input_file, const std::string& output_file);
    ~GameManager();
    
    bool initialize();
    void runGame();

    const GameBoard& getGameBoard() const;
    
private:
    std::string input_file;
    std::string output_file;
    std::ofstream out_stream;
    std::unique_ptr<GameBoard> board;
    std::unique_ptr<TankAlgorithm> algorithm1;
    std::unique_ptr<TankAlgorithm> algorithm2;
    int steps = 0;
    int no_shells_steps = 0;
    bool game_over = false;
    std::string result;
    
    void processStep();
    void processTankAction(const std::shared_ptr<Tank>& tank, const std::string& action);
    void processShells();
    void checkShellCollisions(const std::shared_ptr<Shell>& shell, int x, int y);
    void checkCollisions();
    void checkGameEnd();
    void writeAction(int player_id, const std::string& action, bool is_bad = false);
    void wrapPosition(int& x, int& y);
    void handleWallCollision(const std::shared_ptr<GameObject>& wall);
    void handleShellCollision(const std::shared_ptr<Shell>& shell1, 
                                        const std::shared_ptr<GameObject>& shell2);
    void handleTankCollision(const std::shared_ptr<GameObject>& tank,int x, int y);
    bool shouldDestroyShell(const std::shared_ptr<GameObject>& obj);
    void performBackwardMove(const std::shared_ptr<Tank>& tank);
    void move(Shell& shell);
};