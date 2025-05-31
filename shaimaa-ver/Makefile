# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -Isrc/common -Isrc

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INPUT_FILE = src/input/input.txt

# Find all .cpp files recursively
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")

# Map each .cpp to a .o in the build directory
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))


# Executable name
EXEC = $(BIN_DIR)/tanks_game

# Default target
all: $(EXEC)

# Link the executable
$(EXEC): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp to .o in the build dir
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build and binary files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean run check

# Optional compile check for specific files
CHECK_FILES = src/main/main.cpp src/Game/GameManager/GameManager.cpp
check:
	$(CXX) $(CXXFLAGS) -c $(CHECK_FILES)

# Run with input file
run: $(EXEC)
	./$(EXEC) $(INPUT_FILE)
