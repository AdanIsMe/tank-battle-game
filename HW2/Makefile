# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -Iinclude -Icommon

# Directories
SRC_DIR = src
COMMON_DIR = common
ALGORITHMS_DIR = algorithms
FACTORIES_DIR = myFactories
GAME_DIR = gameCore

# Find all .cpp files in the specified directories
SRCS = $(wildcard $(SRC_DIR)/*.cpp) \
       $(wildcard $(COMMON_DIR)/*.cpp) \
       $(wildcard $(ALGORITHMS_DIR)/*.cpp) \
       $(wildcard $(FACTORIES_DIR)/*.cpp) \
       $(wildcard $(GAME_DIR)/*.cpp)

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable
EXEC = tanks_game

# Default target
all: $(EXEC)

# Link object files into executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean
