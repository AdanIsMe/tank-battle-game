# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic

# Sources and object files
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
EXEC = tanks_game

# Rule to link the object files into an executable
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC)

# Rule to compile each .cpp file into a .o object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(EXEC)

# Phony targets (non-file targets)
.PHONY: clean