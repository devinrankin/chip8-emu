SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/chip8.o $(BUILD_DIR)/graphics.o
TARGET = $(BIN_DIR)/c8emu
FLAGS = -Wall -lSDL3

all: $(TARGET)
	@rm -f $(OBJECTS)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	g++ -o $@ $^ $(FLAGS)
	
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cc $(SRC_DIR)/chip8.hh $(SRC_DIR)/graphics.hh
	@mkdir -p $(BUILD_DIR)		
	g++ -c $< -o $@

$(BUILD_DIR)/chip8.o: $(SRC_DIR)/chip8.cc $(SRC_DIR)/chip8.hh
	@mkdir -p $(BUILD_DIR)
	g++ -c $< -o $@

$(BUILD_DIR)/graphics.o: $(SRC_DIR)/graphics.cc $(SRC_DIR)/graphics.hh
	@mkdir -p $(BUILD_DIR)
	g++ -c $< -o $@

.PHONY: clean
clean:
	@rm -rf  $(BUILD_DIR) $(BIN_DIR)
