# Tool definitions
CC ?= gcc
CXX  ?= g++

# Settings
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
NAME = handheld.elf

# List module source files
MODULE_DIRS = $(wildcard $(SRC_DIR)/*/)
CFLAGS += $(addprefix -I, $(MODULE_DIRS))
CSOURCES = $(SRC_DIR)/main.c
CSOURCES += $(foreach DIR, $(MODULE_DIRS), $(wildcard $(DIR)*.c))

# Compiler flags
CFLAGS += -Wall

# Linker flags
LDFLAGS += 

# Generate names for output object files (*.o)
COBJECTS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(CSOURCES))

# Default rule: build application
.PHONY: all
all: $(BUILD_DIR)/$(NAME)

# Build modules
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build the target app
$(BUILD_DIR)/$(NAME) : $(COBJECTS)
	$(CC) $(COBJECTS) -o $@ $(LDFLAGS)

# Remove compiled object files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*

# Run tests
.PHONY: test
test:
	make -C $(TEST_DIR)

# Clean tests
.PHONY: test_clean
test_clean:
	make -C $(TEST_DIR) clean

