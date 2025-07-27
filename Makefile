## Variables

SOURCE_DIR = src
BUILD_DIR = build
SHADERS_DIR = shaders
BIN_FILE = $(BUILD_DIR)/out

SOURCE_FILES = $(shell find $(SOURCE_DIR) -type f -name "*.cpp")
OBJ_FILES = $(patsubst %,$(BUILD_DIR)/%.o,$(basename $(SOURCE_FILES)))
DEP_FILES = $(patsubst %,$(BUILD_DIR)/%.d,$(basename $(SOURCE_FILES)))
VERTEX_SPV_FILES = $(patsubst %.vert, $(BUILD_DIR)/%.vert.spv, $(shell find $(SHADERS_DIR) -type f -name "*.vert"))
FRAGMENT_SPV_FILES = $(patsubst %.frag, $(BUILD_DIR)/%.frag.spv, $(shell find $(SHADERS_DIR) -type f -name "*.frag"))

# C++ compiler
CXX = clang++
# C++ compiler flags
CXXFLAGS = -std=c++23 -g -Wall -Wextra -O0 -MP -MD
# Pre-processor flags
CPPFLAGS =
# Linker
LD = clang++
# Linker flags
LDFLAGS = #-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
# GLSL compiler
GLSLC = glslc

## Phony targets

.PHONY: all
all: $(BIN_FILE) $(VERTEX_SPV_FILES) $(FRAGMENT_SPV_FILES)

.PHONY: run
run:
	./$(BIN_FILE)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/$(SOURCE_DIR)
	rm -rf $(BUILD_DIR)/$(SHADERS_DIR)

## File targets

$(BIN_FILE): $(OBJ_FILES)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.spv: %
	$(GLSLC) $< -o $@

## Miscellaneous

.PRECIOUS: %/.d

-include $(DEP_FILES)