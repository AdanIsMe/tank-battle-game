#include "../gameCore/GameManager.h"
#include "../include/MyPlayerFactory.h"
#include "../include/MyTankAlgorithmFactory.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <map_file_path>" << std::endl;
        return 1;
    }

    const std::string mapFile = argv[1];

    try {
        GameManager game(MyPlayerFactory{}, MyTankAlgorithmFactory{});
        if (!game.readBoard(mapFile)) {
            std::cerr << "Failed to read board from file: " << mapFile << std::endl;
            return 1;
        }

        game.run(); // Executes the game loop
    } catch (const std::exception& ex) {
        std::cerr << "Exception occurred: " << ex.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}
