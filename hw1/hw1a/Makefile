CC = gcc
CFLAGS = -lm
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .
OUTPUT_DIR = output
ZIP_FILE = raytracer1a.zip

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/raytracer1a

.PHONY: all clean zip

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR) $(OBJ_DIR) $(OUTPUT_DIR)
	$(CC) $^ $(CFLAGS) -o $@
	@rm -rf $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTPUT_DIR) $(TARGET)

zip:
	@zip -r $(ZIP_FILE) $(BIN_DIR) -x $(BIN_DIR)/*.vscode/*