# Compiler and flags as specified
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -Iinclude

# Source files
SRC_DIR = src
ALGORITHMS_DIR = algorithms

SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(ALGORITHMS_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = tanks_game

# Default target
all: $(EXEC)

# Link the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean