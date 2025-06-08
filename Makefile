CC = gcc

# Directories
SRC_DIR = ./src/csrc
INC_DIR = ./src/headers
MAIN_SRC = ./src/main.c

# Object files
OBJ_FILES = user_handling.o file_handling.o

# Target executable
TARGET = main

# Compiler flags
CFLAGS = -I $(INC_DIR) -Wall

# Rules
all: $(TARGET)

$(OBJ_FILES): $(SRC_DIR)/user_handling.c $(SRC_DIR)/file_handling.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/user_handling.c -o user_handling.o
	$(CC) $(CFLAGS) -c $(SRC_DIR)/file_handling.c -o file_handling.o

$(TARGET): $(OBJ_FILES) $(MAIN_SRC)
	$(CC) $(CFLAGS) $(OBJ_FILES) -lcjson $(MAIN_SRC) -o $(TARGET)

# Clean up object files and the executable
clean:
	rm -f $(OBJ_FILES) $(TARGET)

.PHONY: all clean
