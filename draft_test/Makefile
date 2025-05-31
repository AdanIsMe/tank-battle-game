# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -Icommon #REMOVED -WERROR BECAUSE OF COMMON FOLDER

# Source files (all .cpp files in current directory)
SRCS = $(wildcard *.cpp)
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

# Files you want to check
CHECK_FILES = main.cpp GameManager.cpp

# Target to check specific files for compilation errors
check:
	$(CXX) $(CXXFLAGS) -c $(CHECK_FILES)
