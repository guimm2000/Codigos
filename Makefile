all: gg

gg: main.c
	gcc main.c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -o jogo