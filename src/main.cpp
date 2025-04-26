#include "../include/GameManager.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <game_board_input_file>" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = "game_output.txt";
    
    try {
        GameManager gameManager(inputFile, outputFile);
        if (!gameManager.initialize()) {
            return 1;
        }

        // Display initial board state
        std::cout << "\nInitial Game Board:\n";
        gameManager.getGameBoard().displayBoard();
        
        gameManager.runGame();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}