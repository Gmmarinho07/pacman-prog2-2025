# Compilador
CC = gcc

# Diretórios
SRC_DIR = src
INC_DIR = include

# Flags de compilação
CFLAGS = -Wall -Wextra -I$(INC_DIR)

# Flags de linkagem (UCRT64 + RAYLIB)
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Lista de arquivos .c dentro de src/
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Transforma .c → .o
OBJECTS = $(SOURCES:.c=.o)

# Nome do executável
TARGET = pacman.exe

# -------------------------
# REGRAS
# -------------------------

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del $(SRC_DIR)\*.o 2>nul || true
	del $(TARGET) 2>nul || true

run: all
	./$(TARGET)
