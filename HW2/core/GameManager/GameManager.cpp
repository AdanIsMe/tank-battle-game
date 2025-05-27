#include "GameManager.h"
#include <iostream>
#include <fstream>

GameManager::GameManager(int width, int height) 
    : width(width), height(height) {}

GameManager::~GameManager() {}

bool GameManager::loadFromFile(const std::string& file_name, std::string& errors, int num_shells) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "[Error] Failed to open file: " << file_name << "\n";
        return false;
    }

    std::string map_name;
    std::getline(file, map_name); // Skip map name

    int ignored_max_steps, ignored_num_shells;
    if (!(file >> ignored_max_steps >> ignored_num_shells >> rows >> cols)) {
        std::cerr << "[Error] Malformed map metadata in file.\n";
        return false;
    }

    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Skip to next line

    board.clear();
    board.resize(rows, std::vector<char>(cols, ' '));

    std::string line;
    std::string warnings;
    has_warnings = false;

    for (int y = 0; y < rows && std::getline(file, line); ++y) {
        for (int x = 0; x < cols; ++x) {
            char c = (x < (int)line.size()) ? line[x] : ' ';

            if (!isValidCellChar(c)) {
                warnings += "Warning: Invalid character '" + std::string(1, c) +
                            "' at (" + std::to_string(x) + "," + std::to_string(y) +
                            "), treated as space\n";
                c = ' ';
                has_warnings = true;
            }

            switch (c) {
                case '1':
                    tanks.emplace_back(std::make_unique<Tank>(1, x, y, Direction::LEFT, num_shells));
                    player1TankCount++;
                    break;
                case '2':
                    tanks.emplace_back(std::make_unique<Tank>(2, x, y, Direction::RIGHT, num_shells));
                    player2TankCount++;
                    break;
                default:
                    board[y][x] = c;
                    break;
            }
        }
    }

    // Fill missing rows if input had fewer lines
    for (int y = board.size(); y < rows; ++y) {
        board.emplace_back(cols, ' ');
        warnings += "Warning: Missing row " + std::to_string(y) + ", filled with spaces\n";
        has_warnings = true;
    }

    // Save warnings to file
    if (has_warnings && !warnings.empty()) {
        std::ofstream error_file("input_errors.txt");
        if (error_file.is_open()) {
            error_file << warnings;
            error_file.close();
        }
        errors = warnings;
    }

    return true;
}


bool GameManager::isValidCellChar(char c) const {
    return c == '1' || c == '2' || c == '#' || c == '@' || c == ' ';
}

bool GameManager::initialize() {
    try {
        std::ifstream in_file(input_file);
        if (!in_file) {
            std::cerr << "[Error] Could not open input file: " << input_file << "\n";
            return false;
        }
        //ignore first line
        std::string map_name;
        std::getline(in_file, map_name);

        if (!(in_file >> max_steps) || max_steps < 0) {
            std::cerr << "[Error] Invalid maximum steps\n";
            return false;
        }

        if (!(in_file >> num_shells) || num_shells < 0) {
            std::cerr << "[Error] Invalid shell number\n";
            return false;
        }

        if (!(in_file >> rows >> cols) || rows <= 0 || cols <= 0) {
            std::cerr << "[Error] Invalid board dimensions in file\n";
            return false;
        }

        in_file.close();

        std::string load_errors;
        if (!loadFromFile(input_file, load_errors, num_shells)) {
            return false;
        }

        out_stream.open(output_file);
        if (!out_stream) {
            std::cerr << "[Error] Could not open output file: " << output_file << "\n";
            return false;
        }
        
        initializePlayers(board);
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[Error] Initialization exception: " << e.what() << "\n";
        return false;
    }
}

void GameManager::initializePlayers(const vector<vector<char>>& board) {
    // Create player objects with required parameters
    player1 = playerFactory.create(1, cols, rows, max_steps, num_shells);
    player2 = playerFactory.create(2, cols, rows, max_steps, num_shells);

    for (int i = 0; i < player1TankCount; ++i) {
        auto tankAlgo = tankAlgorithmFactory->create(1, i);
        auto tank = std::make_unique<Tank>(1, -1, -1, Direction::RIGHT, num_shells);//(-1,-1) unknown position        tank->setAlgorithm(std::move(tankAlgo));
        player1->addTank(tank);
    }

    int tankCount2 = board.getPlayerTankCount(2);
    for (int i = 0; i < player2TankCount; ++i) {
        auto tankAlgo = tankAlgorithmFactory->create(2, i);
        auto tank = std::make_shared<TankAlgorithm>(2, -1, -1, Direction::LEFT, num_shells);
        tank->setAlgorithm(std::move(tankAlgo));
        player2->addTank(tank);
    }
}

void GameManager::writeBoardToFile(const std::string& file_name) const {
    std::ofstream out_file(file_name);
    if (!out_file) {
        std::cerr << "Error: Unable to open file for writing: " << file_name << std::endl;
        return;
    }

    out_file << width << "x" << height << std::endl;
    std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));

    for (const auto& wall : walls) {
        auto [x, y] = wall->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '#';
        }
    }

    for (const auto& mine : mines) {
        auto [x, y] = mine->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '@';
        }
    }

    for (const auto& shell : shells) {
        auto [x, y] = shell->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '*';
        }
    }

    for (const auto& tank : tanks) {
        auto [x, y] = tank->getPosition();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = '0' + tank->getPlayerId();
        }
    }

    for (int y = 0; y < height; ++y) {
        out_file << std::string(grid[y].begin(), grid[y].end()) << std::endl;
    }

    out_file << "Directions:\n";
    for (const auto& tank : tanks) {
        out_file << tank->getPlayerId() << ": " << DirectionUtil::toString(tank->getDirection()) << std::endl;
    }

    out_file.close();
    std::cout << "Game board saved to " << file_name << std::endl;
}


void GameManager::displayBoard() const {
    std::cout << "\nCurrent Board (" << width << "x" << height << "):\n";
    std::cout << "-------------------------\n";
    std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));

    for (const auto& shell : shells) {
        auto [x, y] = shell->getPosition();
        if (x >= 0 && x < cols && y >= 0 && y < rows) {
            grid[y][x] = '*';
        }
    }

    for (const auto& tank : tanks) {
        auto [x, y] = tank->getPosition();
        if (x >= 0 && x < cols && y >= 0 && y < rows) {
            grid[y][x] = '0' + tank->getPlayerId();
        }
    }

    std::cout << "   ";
    for (int x = 0; x < cols; ++x) {
        std::cout << " " << x;
    }
    std::cout << "\n";

    for (int y = 0; y < rows; ++y) {
        std::cout << std::setw(2) << y << " ";
        for (int x = 0; x < cols; ++x) {
            std::cout << " " << grid[y][x];
        }
        std::cout << "\n";
    }

    std::cout << "\nLegend:\n"
              << "  #: Wall\n"
              << "  @: Mine\n"
              << "  o: Shell\n"
              << "  1: Player 1 Tank\n"
              << "  2: Player 2 Tank\n";
}

void GameManager::addShell(std::shared_ptr<Shell> shell) {
    active_shells.push_back(shell);
}

void GameManager::removeShell(std::shared_ptr<Shell> shell) {
    active_shells.erase(std::remove(active_shells.begin(), active_shells.end(), shell), active_shells.end());
}

void GameBoard::clear() {
    board.clear();
    tanks.clear();
    active_shells.clear();
}