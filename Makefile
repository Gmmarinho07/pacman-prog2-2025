# Compilador
CC = gcc

# Diretórios
SRC_DIR = src
INC_DIR = include

# Flags de compilação
CFLAGS = -Wall -Wextra -I$(INC_DIR)

# Flags de linkagem (Windows + Raylib)
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows

# Arquivos fonte
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Objetos
OBJECTS = $(SOURCES:.c=.o)

# Executável
TARGET = pacman.exe

# Regras
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
