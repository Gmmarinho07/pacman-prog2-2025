# pacman-prog2-2025
Trabalho prático de Prog II - Pacman implementado em C
#Compilador

CC = gcc
CFLGS = -Wall -Wextra -std=c99 - g
LIBS = lraylib -lwinm -lgdi32 -lopeng32

#Diretorios

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

#Araquivos fonte e obejtos
SOURCE = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCE: $(SRC_DIR)/%.c=$(OBJ_DIR)/%.c)

#Nome do executável
TARGET = $(BIN_DIR) / pacman

# Regra principal
all: $(TARGET)

$(TARGET): $(OBJETCS) | $(BIN_DIR)
    $(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

$(OBJ_DIR) /%.c: $(SRC_DIR)/%.c | $(OBJ_DIR)
    $(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@


    