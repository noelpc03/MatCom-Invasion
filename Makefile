# MatCom Invasion - Makefile
# ============================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I./include -pthread
LDFLAGS = -lncurses -lpthread

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin
DATA_DIR = data

# Target executable
TARGET = $(BIN_DIR)/matcom-invasion

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DATA_DIR)

# Link the executable
$(TARGET): $(OBJS)
	@echo "Linking $@..."
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build successful!"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Run the game
run: all
	@./$(TARGET)

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)/*.o $(TARGET)
	@echo "Clean complete!"

# Deep clean (remove all generated files and directories)
distclean: clean
	@echo "Deep cleaning..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Deep clean complete!"

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	@sudo apt-get update
	@sudo apt-get install -y libncurses5-dev libncursesw5-dev gcc make
	@echo "Dependencies installed!"

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: clean all

# Release build (optimized)
release: CFLAGS += -O2
release: clean all

# Help
help:
	@echo "MatCom Invasion - Makefile Help"
	@echo "================================"
	@echo "Available targets:"
	@echo "  all          - Build the game (default)"
	@echo "  run          - Build and run the game"
	@echo "  clean        - Remove build artifacts"
	@echo "  distclean    - Remove all generated files"
	@echo "  install-deps - Install required dependencies"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized version"
	@echo "  help         - Show this help message"

.PHONY: all directories run clean distclean install-deps debug release help
