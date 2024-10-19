# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
DIST_DIR = dist

# Target and version info
TARGET = ksh
VERSION = 0.1.1

# Determine the build type
ifneq ($(type), RELEASE)
	CFLAGS += -DDEBUG -ggdb
else
	CFLAGS += -O3
endif

# Source and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Default target
.DEFAULT_GOAL := help

# Total source file count
TOTAL_FILES := $(words $(SRC_FILES))

# Counter to track progress
counter = 0

# Targets

## Build the project
.PHONY: all
all: check_tools $(BUILD_DIR) $(TARGET) ## Build the project
	@echo "Build complete."

## Check required tools (like bear for compile_commands.json generation)
.PHONY: check_tools
check_tools: ## Check if necessary tools are available
	@command -v gcc >/dev/null 2>&1 || { echo >&2 "[ERRO] gcc is not installed."; exit 1; }
	@command -v bear >/dev/null 2>&1 || { echo >&2 "[WARN] bear is not installed. Skipping compile_commands.json target."; }

## Create the build directory
$(BUILD_DIR): ## Create the build directory if it doesn't exist
	@echo "[INFO] Creating build directory"
	mkdir -p $(BUILD_DIR)

## Build the executable from object files
$(TARGET): $(OBJ_FILES) ## Build the shell executable
	@echo "[INFO] Building the shell"
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "[INFO] Executable created: $(TARGET)"

## Compile source files into object files with progress tracking
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c ## Compile source files with progress
	$(eval counter=$(shell echo $$(($(counter)+1))))
	@echo "[$(counter)/$(TOTAL_FILES)] Compiling $< -> $@"
	$(CC) $(CFLAGS) -c -o $@ $<

## Install the executable
.PHONY: install
install: all ## Install the executable to /usr/bin/
	@echo "[INFO] Installing $(TARGET) to /usr/bin/"
	cp $(TARGET) /usr/bin/$(TARGET)

## Uninstall the executable
.PHONY: uninstall
uninstall: ## Remove the executable from /usr/bin/
	@echo "[INFO] Uninstalling $(TARGET)"
	rm -f /usr/bin/$(TARGET)

## Clean object files and executable
.PHONY: clean
clean: ## Remove all build files and the executable
	@echo "[INFO] Cleaning up build directory and executable."
	rm -rf $(BUILD_DIR) $(TARGET)

## Remove everything, including object files and editor backup files
.PHONY: distclean
distclean: clean ## Perform a full clean, including backup and temporary files
	@echo "[INFO] Performing full clean, removing build directory, dist files, and editor backups."
	rm -f *~ core $(SRC_DIR)/*~ $(DIST_DIR)/*.tar.gz

## Create a tarball of the project
.PHONY: dist
dist: $(SRC_FILES) ## Create a tarball of the project
	@echo "[INFO] Creating a tarball for version $(VERSION)"
	mkdir -p $(DIST_DIR)
	tar -czvf $(DIST_DIR)/$(TARGET)-$(VERSION).tar.gz $(SRC_DIR) $(INCLUDE_DIR) Makefile README.md

## Generate compile_commands.json
.PHONY: compile_commands.json
compile_commands.json: $(SRC_FILES) ## Generate compile_commands.json
	@echo "[INFO] Generating compile_commands.json"
	bear -- make all

## Show this help message
.PHONY: help
help: ## Show this help message
	@echo "Available commands:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'

## Enable verbose output for debugging
.PHONY: verbose
verbose: CFLAGS += -DVERBOSE
verbose: all ## Build the project in verbose mode

# Phony targets to avoid conflicts with file names
.PHONY: all clean distclean install uninstall dist compile_commands.json help check_tools verbose
