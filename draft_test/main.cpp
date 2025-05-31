#include <iostream>
#include <string>
#include "GameManager.h"
#include "MyPlayerFactory.h"
#include "MyTankAlgorithmFactory.h"

int main(int argc, char** argv) {
    // Check command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <game_board_input_file>" << std::endl;
        return 1;
    }
    
    // Get input file name
    std::string input_file = argv[1];
    
    // Create game manager with factories
    MyTankAlgorithmFactory tank_factory;
    MyPlayerFactory player_factory;
    GameManager game(player_factory, tank_factory);
    
    // Read the board
    game.readBoard(input_file);
    
    // Set output file
    std::string output_file = "game_output.txt";
    game.writeOutput(output_file);
    
    // Run the game
    game.runGame();
    
    std::cout << "Game completed. Results written to " << output_file << std::endl;
    
    return 0;
}
