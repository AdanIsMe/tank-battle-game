#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>
#include "HelperComponents.h"
#include "GameBoard.h"
#include "GameObject.h"
#include "Tank.h"
#include "Wall.h"
#include "Mine.h"
//#include "Shell.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <game_board_input_file>\n";
        return 1;
    }

    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Failed to open input.txt" << std::endl;
        return 1;
    }

    try
    {
        int width, height;
        // read the width and height of the game board from the first line
        file >> width >> height;
        file.ignore(); // Skip the newline after reading width and height (to avoid getting stuck)

        GameBoard board(width, height);

        for (int row = 0; row < width; ++row) {
            std::string line;
            std::getline(file, line);
    
            //if (line.length() != static_cast<size_t>(height)) {
            //    std::cerr << "Line " << row + 1 << " has incorrect length." << std::endl;
            //    return 1;
            //}
    
            for (int col = 0; col < height; ++col) {
                char symbol = line[col];
                Position pos(row, col);
                std::unique_ptr<GameObject> obj;
    
                switch (symbol) {
                    //case 'E': obj = std::make_unique<EmptySpace>(); break;
                    case '1': obj = std::make_unique<Tank>(); break;
                    case '2': obj = std::make_unique<Tank>(); break;
                    case '@': obj = std::make_unique<Mine>(); break;
                    case '#': obj = std::make_unique<Wall>(); break;
                    //default:
                    //    std::cerr << "Invalid character '" << symbol << "' at (" << row << "," << col << ")" << std::endl;
                    //    return 1;
                }
    
                obj->setPosition(pos);                
                board.setObjectAt(pos, std::move(obj));
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
  
