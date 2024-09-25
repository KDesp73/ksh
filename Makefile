CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS =

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

ifneq ($(type), RELEASE)
	CFLAGS += -DDEBUG -ggdb
endif

# List all the source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Generate the corresponding object file names
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Target: the final executable
TARGET = ksh

# Default target, build the executable
all: $(BUILD_DIR) $(TARGET)

# Rule to create the build directory
$(BUILD_DIR):
	@echo "Creating build directory"
	mkdir -p $(BUILD_DIR)

# Rule to build the executable
$(TARGET): $(OBJ_FILES)
	@echo "Building the shell"
	$(CC) -o $@ $^ $(LDFLAGS)


# Rule to build object files from source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

install: all
	@echo "Installing..."
	cp $(TARGET) /usr/bin/$(TARGET)

uninstall:
	@echo "Uninstalling..."
	rm /usr/bin/$(TARGET)

# Clean rule to remove generated files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

compile_commands.json: $(SRC_FILES)
	bear -- make


# Phony target to avoid conflicts with file names
.PHONY: all clean

