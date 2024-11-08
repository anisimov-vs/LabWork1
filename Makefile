# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Werror -Wpedantic -Wall -std=c++17 -I$(SRC_DIR)
LDFLAGS = -pthread

# Directories
SRC_DIR = src
OBJ_DIR = tmp
BIN_DIR = .

# Find all .cpp files in the src directory
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Generate corresponding .o files in the tmp directory
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Target binary
TARGET = $(BIN_DIR)/main

# Default target
all: $(TARGET)

# Link the object files to create the binary
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .cpp file to a .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up the build
clean:
	rm -rf $(OBJ_DIR)

cleanall: clean
	rm -rf $(TARGET)

# Phony targets
.PHONY: all clean
