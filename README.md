# pacman-prog2-2025
Trabalho prático de Prog II - Pacman implementado em C


Este é um projeto de implementação do jogo Pac-Man em C utilizando a biblioteca gráfica Raylib.

## Requisitos

Para compilar e rodar, você deve ter instalados:
1.  Um compilador C (GCC).
2.  A biblioteca Raylib configurada para linkagem.

## Estrutura do Projeto

O projeto segue a estrutura modular:
- `include/`: Arquivos de cabeçalho (.h) (e.g., entities.h, map.h, game.h, game_data.h).
- `src/`: Arquivos fonte (.c) (e.g., main.c, game.c, entities.c, map.c, game_data.c).
- `Makefile`: Script para compilação.

## Compilação e Execução

O `Makefile` usa `gcc` e flags específicas para Raylib no ambiente Windows.

### 1. Compilação
Abra o terminal na pasta raiz do projeto e execute:
```bash
make all

make run
# ou diretamente:
./pacman.exe