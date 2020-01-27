#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define LARGURA 800
#define ALTURA 600
#define FPS 60
#define SPEED 3
#define LARGURA_PLAYER 66
#define ALTURA_PLAYER 68

#define OPENING 0
#define MENU 1
#define OPTIONS 2
#define RANKING 3
#define CREDITS 4
#define HISTORIA 5
#define JOGO 6
#define PAUSE 7
#define STATUS 8
#define INVENTORY 9
#define FALA 10

#define NEW_GAME 1
#define LOAD_GAME 2
#define OPCOES 3
#define SCORE 4
#define CREDITOS 5
#define QUIT 6

#define PAUSE_RESUME 1
#define PAUSE_STATUS 2
#define PAUSE_INVENTORY 3
#define PAUSE_SAVE 4
#define PAUSE_LOAD 5
#define PAUSE_OPTIONS 6
#define PAUSE_QUIT 7

typedef struct {
	int defesa;
	int valor;
} Armadura; //struct da armadura

typedef struct {
	int dano, valor;
} Arma; //struct da arma

typedef struct {
	SDL_Rect dest, source;
	SDL_Texture *text;
	char nome[11];
	unsigned int pontos, vida, dano, armadura, mana;
	int speed_x, speed_y, up, down, left, right, direction;
	int posxmapa, posymapa;
	int acao, soco, magia;
	int magiaeq, espadaeq, elmofeq, peitoralfeq, botafeq;
	int elmoceq, peitoralceq, botaceq;
	int espada, pocaov, pocaom, capacetef, peitoralf, botasf, capacetec, peitoralc, botasc;
} Player; //struct do player

typedef struct {
	SDL_Rect rect, source;
	SDL_Texture *text;
	int direction;
} NPC; //struct de npcs

typedef struct {
	SDL_Rect rect;
	SDL_Texture *text;
	int speed_x, speed_y;
} Inimigo; //struct dos inimigos

char *alocaChar(int tam);
void desalocChar(char *vet);
SDL_Texture *loadText(SDL_Renderer *r, char *path); //Retorna uma textura do SDL
void fadeIn(SDL_Window *w, SDL_Renderer *r, char *path); //Da efeito de aparecer
void fadeOut(SDL_Window *w, SDL_Renderer *r, char *path); //Da efeito de sumir
void fadeInHistoria(SDL_Window *w, SDL_Renderer *r, char *path, SDL_Rect pos); //Mostra a historia
void printTexto(SDL_Renderer *r, SDL_Color color, TTF_Font *font, SDL_Rect pos, char *frase); //Printa um texto na tela
void fadeInMenu(SDL_Window *w, SDL_Renderer *r, char *path); //Mostra o Menu
void sortRanking(Player *ranking); // \Organiza o ranking
void animation(SDL_Rect *source, int direction); //Anima a sprite do personagem
int colisaoCima(SDL_Rect *player, SDL_Rect *objeto);
int colisaBaixo(SDL_Rect *player, SDL_Rect *objeto);

SDL_Window *win = NULL;
SDL_Renderer *render = NULL;
SDL_Event e;
TTF_Font *font = NULL;

int main() {
	int jogando = 1, game_state = OPENING, frame_delay = 1000/FPS, frame_time, frame_start, inicio = 0, i, j, cont = 0, pause_state = 1;
	int aperta = 0, game_mode = 0, fala = 0, proxfala, inventcontrol = 1, selecao = 0;
	int state = NEW_GAME;
	char *def, *dano, *pontos;
	Player player;
	NPC guardas[4];
	NPC guardasdem[2];
	NPC oraculo;
	FILE *pa;
	SDL_Texture *moedatext, *menutext, *Backgroundtext, *pausetext, *indicadortext;
	SDL_Rect moedarect;
	Player ranking[4];
	SDL_Rect numeros[3];
	SDL_Rect moedaVoltar;
	SDL_Texture *mapabacktext;
	SDL_Texture *statustext;

	player.posxmapa = -3210; player.posymapa = -650;

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Erro: %s\n", SDL_GetError());
		jogando = 0;
	}

	if(TTF_Init() < 0) {
		printf("Erro: %s\n", SDL_GetError());
		jogando = 0;
	}

	win = SDL_CreateWindow("HumanSlayer", 100, 100, LARGURA, ALTURA, 0);

	if(win == NULL) {
		printf("Erro: %s\n", SDL_GetError());
		jogando = 0;
	}

	render = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC || SDL_RENDERER_ACCELERATED);

	if(render == NULL) {
		printf("Erro: %s\n", SDL_GetError());
		jogando = 0;
	}

	//Inicializa o ranking e os pontos
	pa = fopen("Ranking.txt", "a+");		
	for(i = 0; i < 4; i++) {
		fscanf(pa, "%s", ranking[i].nome);
	}
	fclose(pa);

	pa = fopen("Pontos.txt", "a+");

	for(i = 0; i < 4; i++) {
		fscanf(pa, "%d", &ranking[i].pontos);
	}

	fclose(pa);

	sortRanking(ranking);

	//Inicializa todas as variaveis

	SDL_Color vermelho = {255, 0, 0, 255};
	SDL_Color branco = {255, 255, 255, 255};
	SDL_Color marrom = {92, 51, 23, 255};
	SDL_Rect indicadorrect;	indicadorrect.w = 10;	indicadorrect.h = 10;	indicadorrect.x = 610;	indicadorrect.y = 55;
	SDL_Rect pauserect;	pauserect.w = 200;	pauserect.h = 600;	pauserect.x = (LARGURA - pauserect.w);	pauserect.y = 0; 
	SDL_Rect voltar;	voltar.w = 100;		voltar.x = 100;		voltar.h = 50;		voltar.y = 500;
	SDL_Rect titulorect;	titulorect.w = 400;		titulorect.h = 100;		titulorect.x = (LARGURA - titulorect.w)/2;	titulorect.y = 50;
	SDL_Rect telameio;	telameio.w = 600;	telameio.h = 100;	telameio.x = (LARGURA - telameio.w)/2;	telameio.y = (ALTURA - telameio.h)/2;
	SDL_Rect historiarect; historiarect.w = 800;	historiarect.h = 1480;		historiarect.x = 0;		historiarect.y = 20;
	SDL_Rect voltar_status;	voltar_status.x = 600, voltar_status.y = 500;	voltar_status.w = 25;	voltar_status.h = 25;
	SDL_Rect mapaback;	mapaback.x = -3270;	mapaback.y = -730;	mapaback.w = 5000;	mapaback.h = 2000;
	SDL_Rect player_status, player_status_source, danorect, defrect, vidarect;
	SDL_Rect mapafore; mapafore.x = -3270; mapafore.y = -730; mapafore.w = 5000; mapafore.h = 2000;
	SDL_Rect castelodemonrect;
	SDL_Rect selecao_inv;	selecao_inv.x = 450;	selecao_inv.y = 20;		selecao_inv.h = 25;	selecao_inv.w = 25;
	SDL_Rect iteminvent;	iteminvent.x = 90; iteminvent.y = 380;	iteminvent.h = 200;	iteminvent.w = 200;
	SDL_Texture *itemmostra;
	SDL_Texture *historiatext = loadText(render, "Resources/Historia.png");
	SDL_Texture *mapaforetext = loadText(render, "Resources/Mapafore.png");
	SDL_Texture *castelodemontext = loadText(render, "Resources/Castelo_demon.png");
	SDL_Rect reidemonrect;
	SDL_Texture *castelo_part1 = loadText(render, "Resources/Castelo1.png");
	SDL_Rect part1;
	SDL_Rect status_capacete, status_peitoral, status_bota, status_espada;
	SDL_Texture *capaceteeq, *peitoraleq, *botaeq, *espadaeq;
	SDL_Texture *guardad = loadText(render, "Resources/guardad.png");
	SDL_Rect guardadrect;	guardadrect.x = 250;	guardadrect.y = 160;	guardadrect.w = 102;	guardadrect.h = 102;
	SDL_Texture *rei_demontext = loadText(render, "Resources/Rei_demon.png");
	SDL_Texture *inventory = loadText (render, "Resources/Inventoy.png");
	pausetext = loadText(render, "Resources/Pause.png");
	statustext = loadText(render, "Resources/Status.png");
	indicadortext = loadText(render, "Resources/Escolhepause.png");
	mapabacktext = loadText(render, "Resources/Backmapa.png");
	oraculo.text = loadText(render, "Resources/Oraculo.png");

	SDL_Texture *falatext;

	guardas[0].text = loadText(render, "Resources/CavaleiroNPC.png");
	guardas[0].source.x = 34;	guardas[0].source.y = 0;	guardas[0].source.h = 34;	guardas[0].source.w = 34;
	guardas[0].rect.x = -1800; 	guardas[0].rect.y = 500;	guardas[0].rect.h = 102;	guardas[0].rect.w = 112;
	guardas[1].text = loadText(render, "Resources/CavaleiroNPC.png");
	guardas[1].source.x = 0;	guardas[1].source.y = 0;	guardas[1].source.h = 34;	guardas[1].source.w = 34;
	guardas[1].rect.x = -1800; 	guardas[1].rect.y = 700;	guardas[1].rect.h = 102;	guardas[1].rect.w = 112;
	guardas[2].text = loadText(render, "Resources/CavaleiroNPC.png");
	guardas[2].source.x = 34;	guardas[2].source.y = 0;	guardas[2].source.h = 34;	guardas[2].source.w = 34;
	guardas[2].rect.x = -3050; 	guardas[2].rect.y = 100;	guardas[2].rect.h = 102;	guardas[2].rect.w = 112;
	guardas[3].text = loadText(render, "Resources/CavaleiroNPC.png");
	guardas[3].source.x = 34;	guardas[3].source.y = 0;	guardas[3].source.h = 34;	guardas[3].source.w = 34;
	guardas[3].rect.x = -2800; 	guardas[3].rect.y = 100;	guardas[3].rect.h = 102;	guardas[3].rect.w = 112;


	font = TTF_OpenFont("Resources/Purisa.ttf", 20);
	numeros[0].x = 200; numeros[0].y = 200;	numeros[0].h = 50;	numeros[0].w = 50;
	numeros[1].x = 200; numeros[1].y = 250;	numeros[1].h = 50;	numeros[1].w = 50;
	numeros[2].x = 200; numeros[2].y = 300;	numeros[2].h = 50;	numeros[2].w = 50;
	ranking[0].dest.x = 300; ranking[0].dest.y = 200;	ranking[0].dest.h = 50;		ranking[0].dest.w = 200;
	ranking[1].dest.x = 300; ranking[1].dest.y = 250;	ranking[1].dest.h = 50;		ranking[1].dest.w = 200;
	ranking[2].dest.x = 300; ranking[2].dest.y = 300;	ranking[2].dest.h = 50;		ranking[2].dest.w = 200;

	player.dano = 10;	player.vida = 100;	player.armadura = 0;	player.pontos = 0;	player.mana = 200;
	player.espada = 1; player.pocaov = 0;	player.pocaom = 0; player.capacetef = 0; player.peitoralf = 0; player.botasf = 0; player.capacetec = 0; player.peitoralc = 0; player.botasc = 0;
	player.acao = 0;	player.soco = 0;	player.magia = 0;
	player.magiaeq = 0; player.espadaeq = 0; player.elmoceq = 0; player.peitoralceq = 0; player.botaceq = 0; player.elmofeq = 0; player.peitoralfeq = 0; player.botafeq = 0;

	player.text = loadText(render, "Resources/Player.png");
	player.speed_x = player.speed_y = player.up = player.down = player.left = player.right = 0;
	player.dest.w = 66;	player.dest.h = 102;	player.dest.x = (LARGURA - player.dest.w)/2;	player.dest.y = (ALTURA - player.dest.h)/2;
	player.source.w = 22;	player.source.h = 34;	player.source.x = 0;		player.source.y = 0;
	player.direction = 1;
	castelodemonrect.x = -100 + (player.dest.w/2);	castelodemonrect.y = -1100;	castelodemonrect.w = 1000;	castelodemonrect.h = 2000;
	reidemonrect.y = -950;	reidemonrect.x = 325;	reidemonrect.w = 165;	reidemonrect.h = 255;
	part1.x = -100;		part1.y = -345;		part1.h = 750;		part1.w = 1000;
	oraculo.rect.x = 1300;	oraculo.rect.y = 500;	oraculo.rect.w = 102;	oraculo.rect.h = 102;

	moedarect.w = 25;	moedarect.h = 25;	moedarect.x = 10;	moedarect.y = 270;

	SDL_SetRenderDrawColor(render, 0, 0, 0, 255); //Pinta o render de preto

	while(jogando) {
		frame_start = SDL_GetTicks(); //Pega o frame inicial

		if(game_state == OPENING) { //Inicializa o jogo
			SDL_Delay(1000);
			fadeIn(win, render,"Resources/Sdl-logo.png");
			SDL_Delay(1000);
			fadeOut(win, render,"Resources/Sdl-logo.png");
			game_state = MENU;
		}

		else if(game_state == MENU) { //Logica do menu
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			if(inicio == 0) {
				Backgroundtext = loadText(render, "Resources/Background.png");
				menutext = loadText(render, "Resources/HumanSlayer.png");
				moedatext = loadText(render, "Resources/Moeda.png");
				fadeInMenu(win, render, "Resources/Background.png");
			}
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_DOWN:
						state++;
						moedarect.y += 50;
						if(state > QUIT) {
							state = NEW_GAME;
							moedarect.y = 270;
						}
						break;

						case SDL_SCANCODE_UP:
						state--;
						moedarect.y -= 50;
						if(state < NEW_GAME) {
							state = QUIT;
							moedarect.y = 520;
						}
						break;

						case SDL_SCANCODE_RETURN:
						switch(state) {
							case NEW_GAME:
							inicio = 0;
							aperta = 1;
							game_state = HISTORIA;
							break;

							case LOAD_GAME:
							pa = fopen("Save.txt", "r");
							fscanf(pa, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", &game_mode, &guardas[0].rect.x, &guardas[0].rect.y, &guardas[1].rect.x, &guardas[1].rect.y, &guardas[2].rect.x, &guardas[2].rect.y, &guardas[3].rect.x, &guardas[3].rect.y, &player.dest.x, &player.dest.y, &player.vida, &player.armadura, &player.direction, &game_mode, &mapafore.x, &mapafore.y, &castelodemonrect.x, &castelodemonrect.y, &oraculo.rect.x, &oraculo.rect.y, &guardadrect.x, &guardadrect.y);
							fclose(pa);
							inicio = 0;
							aperta = 1;
							game_state = JOGO;
							break;

							case OPCOES:
							game_state = OPTIONS;
							inicio = 0;
							aperta = 1;
							break;

							case SCORE:
							inicio = 0;
							game_state = RANKING;
							aperta = 1;
							break;

							case CREDITOS:
							game_state = CREDITS;
							inicio = 0;
							aperta = 1;
							break;

							case QUIT:
							jogando = 0;
							inicio = 0;
							break;
						}
					}
					break;

					case SDL_KEYUP:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_DOWN:
						state = state;
						break;

						case SDL_SCANCODE_UP:
						state = state;
						break;
					}
					break;

				}
			}
			SDL_RenderClear(render);
			SDL_RenderCopy(render, Backgroundtext, NULL, NULL);
			SDL_RenderCopy(render, menutext, NULL, NULL);
			SDL_RenderCopy(render, moedatext, NULL, &moedarect);
			SDL_RenderPresent(render);
			if(aperta == 0) {
				inicio = 1; //Impede o fade de acontecer de novo e permite mexer no fade dos outros modos
			}
		}

		else if(game_state == HISTORIA) {
			if(inicio == 0) {
				fadeInHistoria(win, render, "Resources/Historia.png", historiarect);
				SDL_Delay(2000);
			}

			cont++;
			if(cont == 2){
				historiarect.y -= 1;
				cont = 0;
			}

			SDL_RenderClear(render);
			SDL_RenderCopy(render, historiatext, NULL, &historiarect);
			SDL_RenderPresent(render);

			if(historiarect.y + historiarect.h < 0) {
				game_state = JOGO;
				inicio = 0;
			}
			inicio = 1;
		}

		else if(game_state == OPTIONS) {
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			moedaVoltar.y = voltar.y + 10;	moedaVoltar.h = moedarect.h;	moedaVoltar.x = voltar.x - 40;	moedaVoltar.w = moedarect.w;

			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						game_state = MENU;
						aperta = 0;
						break;
					}
					break;
				}
			}
			SDL_RenderClear(render);
			printTexto(render, vermelho, font, titulorect, "Options");
			SDL_RenderCopy(render, moedatext, NULL, &moedaVoltar);
			printTexto(render, vermelho, font, voltar, "Voltar");
			SDL_RenderPresent(render);
			inicio = 1;
		}

		else if(game_state == CREDITS) {
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			moedaVoltar.y = voltar.y + 10;	moedaVoltar.h = moedarect.h;	moedaVoltar.x = voltar.x - 40;	moedaVoltar.w = moedarect.w;

			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						game_state = MENU;
						aperta = 0;
						break;
					}
					break;
				}
			}

			SDL_RenderClear(render);
			printTexto(render, vermelho, font, titulorect, "Credits");
			printTexto(render, branco, font, telameio, "Guilherme Montenegro");
			SDL_RenderCopy(render, moedatext, NULL, &moedaVoltar);
			printTexto(render, vermelho, font, voltar, "Voltar");
			SDL_RenderPresent(render);
			inicio = 1;
		}

		else if(game_state == RANKING) {
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			moedaVoltar.y = voltar.y + 10;	moedaVoltar.h = moedarect.h;	moedaVoltar.x = voltar.x - 40;	moedaVoltar.w = moedarect.w;
				
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						game_state = MENU;
						aperta = 0;
						break;
					}
					break;
				}
			}

			SDL_RenderClear(render);
			SDL_RenderCopy(render, moedatext, NULL, &moedaVoltar);
			printTexto(render, vermelho, font, titulorect, "Ranking");
			printTexto(render, vermelho, font, numeros[0], "I");
			printTexto(render, vermelho, font, ranking[0].dest, ranking[0].nome);
			printTexto(render, vermelho, font, numeros[1], "II");
			printTexto(render, vermelho, font, ranking[1].dest, ranking[1].nome);
			printTexto(render, vermelho, font, numeros[2], "III");
			printTexto(render, vermelho, font, ranking[2].dest, ranking[2].nome);
			printTexto(render, vermelho, font, voltar, "Voltar");
			SDL_RenderPresent(render);
			inicio = 1;
		}

		else if(game_state == JOGO) {
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			if(inicio == 0) {
				cont = 0;
			}

			if(player.espadaeq == 1) {
				player.dano = 50;
			}
			else {
				player.dano = 10;
			}

			if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) {
				player.armadura = 20;
				player.source.x = 90;
			}
			else if(player.elmofeq == 1 && player.peitoralfeq == 1 && player.botafeq == 1) {
				player.armadura = 50;
			}
			else {
				player.armadura = 0;
			}
 
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						case SDL_SCANCODE_P:
						case SDL_SCANCODE_ESCAPE:
						game_state = PAUSE;
						inicio = 0;
						break;

						case SDL_SCANCODE_Z:
						if(player.espadaeq == 0 || player.peitoralceq == 0) {
							player.source.x = 66;
							player.soco = 1;
						}
						break;

						case SDL_SCANCODE_DOWN:
						player.down = 1;
						if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 0;
						break;

						case SDL_SCANCODE_UP:
						player.up = 1;
						if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 68;
						break;

						case SDL_SCANCODE_LEFT:
						player.left = 1;
						if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 34;
						break;

						case SDL_SCANCODE_RIGHT:
						if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 102;
						player.right = 1;
						break;

						case SDL_SCANCODE_C:
						player.acao = 1;
						break;
					}
					break;

					case SDL_KEYUP:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_DOWN:
						player.down = 0;
						if(player.peitoralceq == 0) {
							player.source.x = 0;
							player.source.y = 0;
						}
						break;

						case SDL_SCANCODE_UP:
						player.up = 0;
						if(player.peitoralceq == 0) {
							player.source.x = 0;
							player.source.y = player.source.h*2;
						}
						break;

						case SDL_SCANCODE_LEFT:
						player.left = 0;
						if(player.peitoralceq == 0) {
							player.source.x = 0;
							player.source.y = player.source.h;
						}
						break;

						case SDL_SCANCODE_RIGHT:
						player.right = 0;
						if(player.peitoralceq == 0) {
							player.source.x = 0;
							player.source.y = player.source.h*3;
						}
						break;	

						case SDL_SCANCODE_Z:
						if(player.peitoralceq == 0 || player.espadaeq == 0) {
							player.source.x = 0;
							player.soco = 0;
						}
						break;

						case SDL_SCANCODE_C:
						player.acao = 0;
						break;
					}
					break;
				}
			}

			player.speed_y = player.speed_x = 0;
			cont++;

			//Movimentacao e animacao do Player
			if(player.left && !player.right && !player.up && !player.down) { 
				player.speed_x = -SPEED;
				player.direction = 2;
				if(player.peitoralceq == 0) {
					if(cont > 12) {
						animation(&player.source, player.direction);
						cont = 0;
					}
				}
			}

			else if(!player.left && player.right && !player.up && !player.down) {
				player.speed_x = SPEED;
				player.direction = 4;
				if(player.peitoralceq == 0) {
					if(cont > 12) {
						animation(&player.source, player.direction);
						cont = 0;
					}
				}
			}
			
			else if(player.up && !player.down && !player.left && !player.right) {
				player.speed_y = -SPEED;
				player.direction = 3;
				if(player.peitoralceq == 0) {
					if(cont > 12) {
						animation(&player.source, player.direction);
						cont = 0;
					}
				}
			}
			
			else if(!player.up && player.down && !player.left && !player.right) {
				player.speed_y = SPEED;
				player.direction = 1;
				if(player.peitoralceq == 0) {
					if(cont > 12) {
						animation(&player.source, player.direction);
						cont = 0;
					}
				}
			}

			if(game_mode == 0) {
				castelodemonrect.y -= player.speed_y;
				reidemonrect.y -= player.speed_y;

				if(castelodemonrect.y >= -380) {
					castelodemonrect.y += player.speed_y;
					reidemonrect.y += player.speed_y;
				}

				if(castelodemonrect.y >= -390) {
					if(player.acao == 1) {
						if(cont > 50) {
							game_state = FALA;
							fala = 1;
							inicio = 0;
							cont = 0;
						}
					}
				}

				else if(castelodemonrect.y < -1325) {
					game_mode = 1;
				}

				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelodemontext, NULL, &castelodemonrect);
				SDL_RenderCopy(render, rei_demontext, NULL, &reidemonrect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderPresent(render);

			}

			else if(game_mode == 1) {
				mapafore.x -= player.speed_x;
				mapafore.y -= player.speed_y;
				mapaback.x -= (player.speed_x);
				mapaback.y -= (player.speed_y);
				if(mapafore.x <= -3210 && mapafore.x >= -3330 && mapafore.y > -650) {
					game_mode = 0;
				}
				if(mapafore.x >= -96 && mapafore.x <= 63 && mapafore.y >= -555) {
					game_mode = 2;
					part1.y = -345;
				}
				else if(mapafore.x <= -3330 && mapafore.y > -660 && mapafore.x >= -4600) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x <= -4044 && mapafore.x >= -4425 && mapafore.y <= -760 && mapafore.y >= -970) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				else if(mapafore.x >= -3210 && mapafore.y > -660 && mapafore.x <= -2900) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x <= -4584 && mapafore.y <= -661) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x <= -3936 && mapafore.y <= -1360) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x <= -3633 && mapafore.y <= -1399) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x >= -3000 && mapafore.y >= -850 && mapafore.x <= -2724){
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x <= -2100 && mapafore.x >= -2676 && mapafore.y >= -886){
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.y <= -1126 && mapafore.x >= -2100 && mapafore.x <= -1850) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.y >= -967 && mapafore.x >= -2100 && mapafore.x <= -1850) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x <= -1026 && mapafore.x >= -1881 && (mapafore.y >= -1000 || mapafore.y <= -1084)) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				} 
				if(mapafore.x <= -2739 && mapafore.x >= 3621 && mapafore.y <= 1369) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x >= -3567 && mapafore.x <= -2880 && mapafore.y <= -1048) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x <= -2103 && mapafore.x >= -2865 && mapafore.y <= -1204) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x <= -3560 && mapafore.x >= -3666 && mapafore.y <= -1372) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x >= 384 || mapafore.y <= -1645) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}

				if(mapafore.x <= -975 && mapafore.x >= -996 && mapafore.y <= -1987) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.x <= -975 && mapafore.x >= -990 && mapafore.y >= -1009) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.y >= -559 && mapafore.x >= -1000 && mapafore.x <= -102) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}
				if(mapafore.y >= -559 && mapafore.x >= 75 && mapafore.x <= 390) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}

				if(mapafore.x <= -4203 && mapafore.x >= -4251 && mapafore.y >= -988) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}

				if(mapafore.y <= -1135 && mapafore.y >= -1555 && mapafore.x >= -1023 && mapafore.x <= -315) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}

				if(mapafore.y >= -1027 && mapafore.x <= -84 && mapafore.x >= -900) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}

				if(mapafore.x <= 381 && mapafore.x >= 78 && mapafore.y >= -1033 && mapafore.y <= -718) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
				}

				if(mapafore.x >= -1134) {
					if(player.elmoceq != 1 && player.peitoralceq != 1 && player.botaceq != 1) {
						player.left = 0;
						player.direction = 4;
						fala = 6;
						game_state = FALA;
						falatext = loadText(render, "Resources/GuardasFala.png");
					}
				}

				if(mapafore.x <= -4203 && mapafore.x >= -4251 && mapafore.y >= -1000) {
					if(cont > 50) {
						if(player.acao == 1) {
							fala = 3;
							game_state = FALA;
							player.magiaeq = 1;
							falatext = loadText(render, "Resources/OraculoFala1.png");
						}
					}
				}

				if(mapafore.x <= -3147 && mapafore.y >= -685 && mapafore.x >= -3201) {
					if(cont > 50) {
						if(player.acao == 1) {
							fala = 7;
							game_state = FALA;
							player.capacetec = 1;	player.peitoralc = 1;	player.botasc = 1; player.pocaov = 5;	player.pocaom = 5;
							falatext = loadText(render, "Resources/GuardadFala1.png");
						}
					}
				}

				guardas[0].rect.x -= player.speed_x;
				guardas[0].rect.y -= player.speed_y;
				guardas[1].rect.x -= player.speed_x;
				guardas[1].rect.y -= player.speed_y;
				guardas[2].rect.x -= player.speed_x;
				guardas[2].rect.y -= player.speed_y;
				guardas[3].rect.x -= player.speed_x;
				guardas[3].rect.y -= player.speed_y;

				oraculo.rect.x -= player.speed_x;
				oraculo.rect.y -= player.speed_y;
				guardadrect.x -= player.speed_x;
				guardadrect.y -= player.speed_y;

				castelodemonrect.y = -1310;
				printf("x = %d\t y = %d\n", mapafore.x, mapafore.y);

				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderPresent(render);
			}

			else if(game_mode == 2) {
				part1.x -= player.speed_x;
				part1.y -= player.speed_y;
				printf("x = %d\t y = %d\n", part1.x, part1.y);

				if(part1.x <= 20 && part1.x >= -217 && part1.y <= -405) {
					game_mode = 1;
				}

				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelo_part1, NULL, &part1);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderPresent(render);
			}

			inicio = 1;
		}

		else if(game_state == PAUSE) {
			aperta = 1;
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_DOWN:
						indicadorrect.y += 70;
						pause_state++;
						if(pause_state > PAUSE_QUIT) {
							pause_state = PAUSE_RESUME;
							indicadorrect.y = 55;
						}
						break;
						
						case SDL_SCANCODE_UP:
						indicadorrect.y -= 70;
						pause_state--;
						if(pause_state < PAUSE_RESUME) {
							pause_state = PAUSE_QUIT;
							indicadorrect.y = 475; 
						}
						break;

						case SDL_SCANCODE_P:
						case SDL_SCANCODE_ESCAPE:
						game_state = JOGO;
						break;

						case SDL_SCANCODE_RETURN:
						switch(pause_state) {
							case PAUSE_RESUME:
							game_state = JOGO;
							break;

							case PAUSE_STATUS:
							dano = alocaChar(3);	def = alocaChar(3);
							inicio = 0;
							aperta = 0;
							game_state = STATUS;
							break;

							case PAUSE_INVENTORY:
							inventcontrol = 1;
							selecao_inv.x = 450;
							selecao_inv.y = 20;
							game_state = INVENTORY;
							break;

							case PAUSE_SAVE:
							pa = fopen("Save.txt", "w");
							fprintf(pa, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d", game_mode, guardas[0].rect.x, guardas[0].rect.y, guardas[1].rect.x, guardas[1].rect.y, guardas[2].rect.x, guardas[2].rect.y, guardas[3].rect.x, guardas[3].rect.y, player.dest.x, player.dest.y, player.vida, player.armadura, player.direction, game_mode, mapafore.x, mapafore.y, castelodemonrect.x, castelodemonrect.y, oraculo.rect.x, oraculo.rect.y, guardadrect.x, guardadrect.y);
							fclose(pa);
							break;

							case PAUSE_LOAD:
							pa = fopen("Save.txt", "r");
							fscanf(pa, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", &game_mode, &guardas[0].rect.x, &guardas[0].rect.y, &guardas[1].rect.x, &guardas[1].rect.y, &guardas[2].rect.x, &guardas[2].rect.y, &guardas[3].rect.x, &guardas[3].rect.y, &player.dest.x, &player.dest.y, &player.vida, &player.armadura, &player.direction, &game_mode, &mapafore.x, &mapafore.y, &castelodemonrect.x, &castelodemonrect.y, &oraculo.rect.x, &oraculo.rect.y, &guardadrect.x, &guardadrect.y);
							fclose(pa);
							break;

							case PAUSE_OPTIONS:
							break;

							case PAUSE_QUIT:
							game_state = MENU;
							aperta = 0;
							inicio = 0;
							break;
						}
						break;
					}	
					break;
				}
				if(aperta == 1) {
					inicio = 1;
				}
			}

			SDL_RenderClear(render);
			switch(player.direction) {
				case 1:
				player.source.y = 0;
				break;

				case 2:
				player.source.y = player.source.h;
				break;
				
				case 3:
				player.source.y = player.source.h*2;
				break;		

				case 4:
				player.source.y = player.source.h*3;
				break;
			} 

			if(game_mode == 0) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelodemontext, NULL, &castelodemonrect);
				SDL_RenderCopy(render, rei_demontext, NULL, &reidemonrect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, pausetext, NULL, &pauserect);
				SDL_RenderCopy(render, indicadortext, NULL, &indicadorrect);
				SDL_RenderPresent(render);
			}

			else if(game_mode == 1) { 
				SDL_RenderCopy(render, mapabacktext, NULL, &mapaback);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, pausetext, NULL, &pauserect);
				SDL_RenderCopy(render, indicadortext, NULL, &indicadorrect);
				SDL_RenderPresent(render);
			}
		}

		else if(game_state == STATUS) {
			status_capacete.x = 370;	status_capacete.y = 10;		status_capacete.w = 150;	status_capacete.h = 150;
			status_peitoral.x = 390;	status_peitoral.y = 200;	status_peitoral.w = 150;	status_peitoral.h = 150;
			status_bota.x = 390;	status_bota.y = 400;	status_bota.w = 150;	status_bota.h = 150;
			status_espada.x = 50;	status_espada.y = 200;	status_espada.w = 150;	status_espada.h = 150;

			def[0] = (char)(player.armadura/10) + 48;
			def[1] = (char)(player.armadura%10) + 48;
			def[2] = '\0';
			dano[0] = (char)(player.dano/10) + 48;
			dano[1] = (char)(player.dano%10) + 48;
			dano[2] = '\0';
			defrect.x = 660;	defrect.y = 100;	defrect.w = 90;	defrect.h = 70;
			danorect.x = 660;	danorect.y = 170;	danorect.w = 90; danorect.h = 70;
			voltar_status.x = 610; voltar_status.y = 530;	voltar_status.w = 30;	voltar_status.h = 30;
			player_status.x = 185; player_status.y = 10;	player_status.w = 240;	player_status.h = 550;
			player_status_source.x = 0;	player_status_source.y = 0;	player_status_source.w = player.source.w;	player_status_source.h = player.source.h;

			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						case SDL_SCANCODE_ESCAPE:
						game_state = PAUSE;
						desalocChar(def);
						desalocChar(dano);
						break;
					}
					break;
				}
			}


			SDL_RenderClear(render);
			SDL_RenderCopy(render, statustext, NULL, NULL);
			SDL_RenderCopy(render, moedatext, NULL, &voltar_status);
			SDL_RenderCopy(render, player.text, &player_status_source, &player_status);
			printTexto(render, marrom, font, defrect, def);
			printTexto(render, marrom, font, danorect, dano);
			if(player.elmoceq == 1) {
				capaceteeq = loadText(render, "Resources/Capuz.png");
				SDL_RenderCopy(render, capaceteeq, NULL, &status_capacete);
			}
			else if(player.elmofeq == 1) {
				capaceteeq = loadText(render, "Resources/Elmodeferro.png");
				SDL_RenderCopy(render, capaceteeq, NULL, &status_capacete);
			}
			if(player.peitoralceq == 1) {
				peitoraleq = loadText(render, "Resources/Tunicadecouro.png");
				SDL_RenderCopy(render, peitoraleq, NULL, &status_peitoral);
			}
			else if(player.peitoralfeq == 1) {
				peitoraleq = loadText(render, "Resources/Peitoraldeferro.png");
				SDL_RenderCopy(render, peitoraleq, NULL, &status_peitoral);
			}
			if(player.botaceq == 1) {
				botaeq = loadText(render, "Resources/Botasdecouro.png");
				SDL_RenderCopy(render, botaeq, NULL, &status_bota);
			}
			else if(player.botaceq == 1) {
				botaeq = loadText(render, "Resources/Botasdeferro.png");
				SDL_RenderCopy(render, botaeq, NULL, &status_bota);
			}
			if(player.espadaeq == 1) {
				espadaeq = loadText(render, "Resources/EspadadeFogo.png");
				SDL_RenderCopy(render, espadaeq, NULL, &status_espada);
			}
			SDL_RenderPresent(render);
			inicio = 1;
		}

		else if(game_state == INVENTORY) {


			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_DOWN:
						selecao_inv.y += 50;
						inventcontrol++;
						if(inventcontrol > 10) {
							inventcontrol = 1;
							selecao_inv.x = 450;
							selecao_inv.y = 20;
						} 
						else if(inventcontrol == 10) {
							selecao_inv.x = 640;
							selecao_inv.y = 540;
						}
						break;

						case SDL_SCANCODE_RETURN:
						if(inventcontrol < 10) {
							selecao = 1;
						}
						else if(inventcontrol == 10) {
							game_state = PAUSE;
						}
						break;

						case SDL_SCANCODE_UP:
						selecao_inv.y -= 50;
						inventcontrol --;
						if(inventcontrol < 1) {
							inventcontrol = 10;
							selecao_inv.x = 640;
							selecao_inv.y = 540;
						}
						else if(inventcontrol == 9) {
							selecao_inv.x = 450;
							selecao_inv.y = 420;
						}
						break;

					}

					case SDL_KEYUP:
					switch(e.key.keysym.scancode) {
					case SDL_SCANCODE_RETURN:
					selecao = 0;
					break;
					}
					break;
				}
			}

			SDL_RenderClear(render);
			SDL_RenderCopy(render, inventory, NULL, NULL);
			SDL_RenderCopy(render, moedatext, NULL, &selecao_inv);
			switch(inventcontrol) {
				case 1:
				itemmostra = loadText(render, "Resources/Pocaocura.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(player.pocaov > 0) {
						if(player.vida < 100) {
							player.vida += 20;
							if(player.vida > 100) player.vida = 100;
							player.pocaov--;
						}
					}
				} 
				break;

				case 2:
				itemmostra = loadText(render, "Resources/Pocaomana.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(player.pocaom > 0) {
						if(player.mana < 200) {
							player.mana += 40;
							if(player.mana > 200) player.mana = 200;
							player.pocaom--;
						}
					}
				}
				break;

				case 3:
				itemmostra = loadText(render, "Resources/Capuz.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(player.capacetec == 1) {
						if(player.elmoceq == 0) {
							player.elmoceq = 1;
						}
						else {
							player.elmoceq = 0;
						}
					}
				}
				break;

				case 4:
				itemmostra = loadText(render, "Resources/Tunicadecouro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(player.peitoralc == 1) {
						if(player.peitoralceq == 0) {
							player.peitoralceq = 1;
						}
						else {
							player.peitoralceq = 0;
						}
					}
				}
				break;

				case 5:
				itemmostra = loadText(render, "Resources/Botasdecouro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(player.botasc == 1) {
						if(player.botaceq == 0) {
							player.botaceq = 1;
						}
						else {
							player.botaceq = 0;
						}
					}
				}
				break;

				case 6:
				itemmostra = loadText(render, "Resources/EspadadeFogo.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(player.espada == 1) {
						if(player.espadaeq == 0) {
							player.espadaeq = 1;
						}
						else {
							player.espadaeq = 0;
						}
					}
				}
				break;

				case 7:
				itemmostra = loadText(render, "Resources/Elmodeferro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(player.capacetef == 1) {
						if(player.elmofeq == 0) {
							player.elmofeq = 1;
						}
						else {
							player.elmofeq = 0;
						}
					}
				}
				break;

				case 8:
				itemmostra = loadText(render, "Resources/Peitoraldeferro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(player.peitoralf == 1) {
						if(player.peitoralfeq == 0) {
							player.peitoralfeq = 1;
						}
						else {
							player.peitoralfeq = 0;
						}
					}
				}
				break;

				case 9:
				itemmostra = loadText(render, "Resources/Botasdeferro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(player.botasf == 1) {
						if(player.botafeq == 0) {
							player.botafeq = 1;
						}
						else {
							player.botafeq = 0;
						}
					}
				}
				break;
			}
			SDL_RenderPresent(render);
		}

		else if(game_state = FALA) {

			if(inicio == 0) {
				cont = 0;
			}
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_C:
						proxfala = 1;
						break;
					}
					break;

					case SDL_KEYUP:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_C:
						proxfala = 0;
						break;
					}
					break;
				}
			}
			cont++;

			if(fala == 1) {
				falatext = loadText(render, "Resources/ReiFala1.png");
				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelodemontext, NULL, &castelodemonrect);
				SDL_RenderCopy(render, rei_demontext, NULL, &reidemonrect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);
				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						falatext = loadText(render, "Resources/ReiFala2.png");
						fala = 2;
					}
				}
			}

			else if(fala == 2) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelodemontext, NULL, &castelodemonrect);
				SDL_RenderCopy(render, rei_demontext, NULL, &reidemonrect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						game_state = JOGO;
					}
				}
			}

			else if(fala == 3) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapabacktext, NULL, &mapaback);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						falatext = loadText(render, "Resources/OraculoFala2.png");
						fala = 4;
					}
				}
			}

			else if(fala == 4) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapabacktext, NULL, &mapaback);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						falatext = loadText(render, "Resources/OraculoFala3.png");
						fala = 5;
					}
				}
			}

			else if(fala == 5) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapabacktext, NULL, &mapaback);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						game_state = JOGO;
					}
				}
			}

			else if(fala == 6) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapabacktext, NULL, &mapaback);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						game_state = JOGO;
						mapafore.x -= 50;
						mapaback.x -= 50;
						guardas[0].rect.x -= 50;
						guardas[1].rect.x -= 50;
						guardas[2].rect.x -= 50;
						guardas[3].rect.x -= 50;
						oraculo.rect.x -= 50;
					}
				}
			}
			else if(fala == 7) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapabacktext, NULL, &mapaback);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 9;
						falatext = loadText(render, "Resources/Tunica++.png");
					}
				}
			}

			else if(fala == 9) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapabacktext, NULL, &mapaback);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 10;
						falatext = loadText(render, "Resources/Cura++.png");
					}
				}
			}

			else if(fala == 10) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapabacktext, NULL, &mapaback);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 11;
						falatext = loadText(render, "Resources/Mana++.png");
					}
				}
			}

			else if(fala == 10) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapabacktext, NULL, &mapaback);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						game_state = JOGO;
					}
				}
			}

			inicio = 1;
		}		

		//Limita os frames do jogo

		frame_time = SDL_GetTicks() - frame_start; 
		if(frame_delay > frame_time) {
			SDL_Delay(frame_delay - frame_time);
		}

	}
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

char *alocaChar(int tam) {
	char *vet;

	vet = (char*)malloc(sizeof(char)*tam);
	return vet;
}

void desalocChar(char *vet) {
	free(vet);
}

SDL_Texture *loadText(SDL_Renderer *r, char *path) {
	SDL_Surface *temp = IMG_Load(path);
	SDL_Texture *text = SDL_CreateTextureFromSurface(r, temp);
	SDL_FreeSurface(temp);

	return text;
}

void fadeIn(SDL_Window *w, SDL_Renderer *r, char *path) {
	int opacidade = 0;
	SDL_Texture *text;

	while (opacidade < 255) {

		text = loadText(r, path);
		SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(text, opacidade);
		SDL_RenderCopy(r, text, NULL, NULL);
		SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
		SDL_RenderPresent(r);

		opacidade += 2;

		SDL_RenderClear(r);
	}

}

void fadeInHistoria(SDL_Window *w, SDL_Renderer *r, char *path, SDL_Rect pos) {
	int opacidade = 0;
	SDL_Texture *text;

	while (opacidade < 255) {

		text = loadText(r, path);
		SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(text, opacidade);
		SDL_RenderCopy(r, text, NULL, &pos);
		SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
		SDL_RenderPresent(r);

		opacidade += 2;

		SDL_RenderClear(r);
	}

}

void fadeInMenu(SDL_Window *w, SDL_Renderer *r, char *path) {
	int opacidade = 0;
	SDL_Texture *text;

	while (opacidade < 255) {

		text = loadText(r, path);
		SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(text, opacidade);
		SDL_RenderCopy(r, text, NULL, NULL);
		SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
		SDL_RenderPresent(r);

		opacidade += 1;

		SDL_RenderClear(r);
	}

}

void fadeOut(SDL_Window *w, SDL_Renderer *r, char *path) {
	int opacidade = 255;
	SDL_Texture *text;

	while (opacidade > 0) {

		text = loadText(r, path);
		SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(text, opacidade);
		SDL_RenderCopy(r, text, NULL, NULL);
		SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
		SDL_RenderPresent(r);

		opacidade -= 2;

		SDL_RenderClear(r);
	}

	SDL_DestroyTexture(text);
}

void printTexto(SDL_Renderer *r, SDL_Color color, TTF_Font *font, SDL_Rect pos, char *frase) {
	SDL_Surface *textSurf = TTF_RenderText_Solid(font, frase, color);
	SDL_Texture *text;
	text = SDL_CreateTextureFromSurface(r, textSurf);
	SDL_FreeSurface(textSurf);

	SDL_RenderCopy(r, text, NULL, &pos);
}

void sortRanking(Player *ranking) {
	int i, j, temp;
	char tempchar[11];

	for(i = 0; i < 4; i++) {
		if(ranking[i].pontos > ranking[i-1].pontos) {
			temp = ranking[i].pontos;
			ranking[i].pontos = ranking[i-1].pontos;
			ranking[i-1].pontos = temp;

			for(j = 0; j < 11; j++) {
				tempchar[j] = ranking[i].nome[j];
				ranking[i].nome[j] = ranking[i-1].nome[j];
				ranking[i-1].nome[j] = tempchar[j];
			}
			i = 0;
		}
	}
}

void animation(SDL_Rect *source, int direction) {
	switch(direction) {
		case 1:
		source->x += LARGURA_PLAYER/3;
		source->y = 0;
		if(source->x >= LARGURA_PLAYER) {
			source->x = 0;
			source->y = 0;
		}
		break;
		
		case 2:
		source->x += LARGURA_PLAYER/3;
		source->y = source->h;
		if(source->x >= LARGURA_PLAYER) {
			source->x = 0;
			source->y = source->h;
		}
		break;

		case 3:
		source->x += LARGURA_PLAYER/3;
		source->y = source->h*2;
		if(source->x >= LARGURA_PLAYER) {
			source->x = 0;
			source->y = source->h*2;
		}
		break;

		case 4:
		source->x += LARGURA_PLAYER/3;
		source->y = source->h*3;
		if(source->x >= LARGURA_PLAYER) {
			source->x = 0;
			source->y = source->h*3;
		}
		break;
	}
}

int colisaoCima(SDL_Rect *player, SDL_Rect *objeto) {
	int Ax, Bx, Ay, By;
	Ax = player->x;	Ay = player->y;	Bx = (player->x + player->w); By = player->y;
	int Cx, Dx, Cy, Dy;
	Cx = objeto->x;	Dx = (objeto->x + objeto->w); Cy = (objeto->y + objeto->h);	Dy = (objeto->y + objeto->h);

	if((Ax >= Cx && Ay <= Cy && Ax <= Dx && Ay >= Dy) || (Bx >= Cx && By <= Cy && Bx <= Dx && By >= Dy)) {
		return 1;
	}

	else return 0;
}

int colisaBaixo(SDL_Rect *player, SDL_Rect *objeto) {

}