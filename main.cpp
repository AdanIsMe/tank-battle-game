#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>
#include "HelperComponents.h"
#include "GameBoard.h"
#include "GameManager.h"
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
    //GameManager gm;

    if (!file) {
        std::cerr << "Failed to open input.txt" << std::endl;
        return 1;
    }
    
    std::unique_ptr<GameManager> gm;  // Declare pointer here so it's accessible later
    
    try
    {
        int width, height;
        // read the width and height of the game board from the first line

        file >> width >> height;
        file.ignore(); // Skip the newline after reading width and height (to avoid getting stuck)

        GameBoard board(width, height);
        int x1,y1,x2,y2 = 0;

        for (int row = 0; row < width; ++row) {
            std::string line;
            std::getline(file, line);
            for (int col = 0; col < height; ++col) {
                char symbol = line[col];
                Position pos(row, col);
                std::unique_ptr<GameObject> obj;
    
                switch (symbol) {
                    //case 'E': obj = std::make_unique<EmptySpace>(); break;
                    case '1': 
                        //obj = std::make_unique<Tank>(); 
                        //player will create the tankobject
                        x1 = row;
                        y1 = col;
                        break;
                    case '2': 
                        //obj = std::make_unique<Tank>();
                        //player will create the tankobject
                        x2 = row;
                        y2 = col;
                        break;
                    case '@': 
                        obj = std::make_unique<Mine>(); 
                        obj->setPosition(row,col); 
                        board.placeObject(std::move(obj),row,col);  
                        break;
                    case '#': 
                        obj = std::make_unique<Wall>();
                        obj->setPosition(row,col); 
                        board.placeObject(std::move(obj),row,col);   
                        break;
                    //default:
                    //    std::cerr << "Invalid character '" << symbol << "' at (" << row << "," << col << ")" << std::endl;
                    //    return 1;
                }
    
                //obj->setPosition(row,col); 
                //board.placeObject(std::move(obj),row,col);               
                //board.setObjectAt(pos, std::move(obj));
            }
        }
        
        GameManager gm(board&,x1,y1,x2,y2);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    gm->play();
    return 0;
}
  
