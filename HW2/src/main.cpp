#include "GameManager.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }
    std::string input_file = argv[1];
    std::string output_file = "output_" + input_file;

    try {
        GameManager game(argv[1], output_file);

        if (!game.initialize()) {
            std::cerr << "[Error] Game initialization failed — unrecoverable input issue.\n";
            return 1;
        }

        std::cout << "\nInitial Game Board:\n";
        game.getGameBoard().displayBoard();

        game.runGame();
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Fatal Exception] " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "[Fatal Exception] Unknown fatal error occurred\n";
        return 1;
    }
}
