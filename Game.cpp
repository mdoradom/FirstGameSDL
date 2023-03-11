#include "Game.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

Game::Game() {}
Game::~Game(){}

bool Game::Init()
{
	//Initialize SDL with all subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	//Create our window: title, x, y, w, h, flags
	Window = SDL_CreateWindow("Spaceship: arrow keys + space", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (Window == NULL)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}

	//Create a 2D rendering context for a window: window, device index, flags
	Renderer = SDL_CreateRenderer(Window, -1, 0);
	if (Renderer == NULL)
	{
		SDL_Log("Unable to create rendering context: %s", SDL_GetError());
		return false;
	}

	
	//Initialize keys array
	for (int i = 0; i < MAX_KEYS; ++i)
		keys[i] = KEY_IDLE;

	// Load Images
	if (!LoadImages()) {
		return false;
	}

	//Inicialize the list of enemies
	for (int i = 0; i < 10; i++)
	{
		int x = 52;
		int y = rand() % 100 + 1;
		enemies[i].Init(WINDOW_WIDTH - x * i, -y *i, x, 41, 2, -1, 1);
	}


	//Init variables
	Player.Init(0, WINDOW_HEIGHT >> 1, 104, 82, 5);
	idx_shot = 0;
	idx_shotEnemies = 0;
	int w;
	SDL_QueryTexture(background_texture, NULL, NULL, &w, NULL);
	Scene.Init(0, 0, w, WINDOW_HEIGHT, 4);
	god_mode = false;

	return true;
}

bool Game::LoadImages() {
	// Load Images
	//Initialize PNG loading
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		SDL_Log("IMG_Init: only PNG support: %s\n", IMG_GetError());
		return false;
	}

	background_texture = SDL_CreateTextureFromSurface(Renderer, IMG_Load("background.png"));
	if (background_texture == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	shot_texture = SDL_CreateTextureFromSurface(Renderer, IMG_Load("shot.png"));
	if (shot_texture == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	spaceship_texture = SDL_CreateTextureFromSurface(Renderer, IMG_Load("spaceship.png"));
	if (spaceship_texture == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	enemy_texture = SDL_CreateTextureFromSurface(Renderer, IMG_Load("spaceship.png"));
	if (enemy_texture == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	//Load image at specified path

	return true;
}

void Game::Release()
{
	// Release textures images
	SDL_DestroyTexture(background_texture);
	SDL_DestroyTexture(shot_texture);
	SDL_DestroyTexture(spaceship_texture);
	SDL_DestroyTexture(enemy_texture);

	//Clean up all SDL initialized subsystems
	SDL_Quit();
}
bool Game::Input()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)	return false;
	}

	SDL_PumpEvents();
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i])
			keys[i] = (keys[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			keys[i] = (keys[i] == KEY_REPEAT || keys[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}

	return true;
}
bool Game::Update()
{
	//Read Input
	if (!Input())	return true;

	//Process Input
	int fx = 0, fy = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	if (keys[SDL_SCANCODE_F1] == KEY_DOWN) god_mode = !god_mode;
	if (keys[SDL_SCANCODE_UP] == KEY_REPEAT)	fy = -1;
	if (keys[SDL_SCANCODE_DOWN] == KEY_REPEAT)	fy = 1;
	if (keys[SDL_SCANCODE_LEFT] == KEY_REPEAT)	fx = -1;
	if (keys[SDL_SCANCODE_RIGHT] == KEY_REPEAT)	fx = 1;
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN)
	{
		int x, y, w, h;
		Player.GetRect(&x, &y, &w, &h);
		// shots spawn point are [(29, 3), (29, 59)]
		Shots[idx_shot].Init(x + 29, y + 3, 56, 20, 10);
		Shots[idx_shot+1].Init(x + 29, y + 59, 56, 20, 10);
		idx_shot += 2;
		idx_shot %= MAX_SHOTS;
	}

	//Logic
	// Enemy move

	for (int i = 0; i < 10; i++)
	{
		if (enemies[i].GetY() > WINDOW_HEIGHT - 41) { enemies[i].SetMovY(-1); }
		else if (enemies[i].GetY() < 0) { enemies[i].SetMovY(1); }


		if (enemies[i].GetX() > WINDOW_WIDTH - 52) { enemies[i].SetMovX(-1); }
		else if (enemies[i].GetX() < WINDOW_WIDTH /2) { enemies[i].SetMovX(1); }

		enemies[i].Move();
		

		int shoote = rand() % 100 + 1;

		if (shoote < 5 && i%2 == 0) {
			int x, y, w, h;
			enemies[i].GetRect(&x, &y, &w, &h);
			ShotsEnemies[idx_shotEnemies].Init(x - 29, y + 3, 28, 10, 10);
			ShotsEnemies[idx_shotEnemies + 1].Init(x - 29, y + 59, 28, 10, 10);
			idx_shotEnemies += 2;
			idx_shotEnemies %= MAX_SHOTS;
		}

	}







	// Scene Scroll
	Scene.Move(-1, 0);
	if (Scene.GetX() <= -Scene.GetWidth())	Scene.SetX(0);
	//Player update
	Player.Move(fx, fy);
	//Shots update
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].Move(1, 0);
			if (Shots[i].GetX() > WINDOW_WIDTH)	Shots[i].ShutDown();
		}
	}

	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (ShotsEnemies[i].IsAlive())
		{
			ShotsEnemies[i].Move(-1, 0);
			if (ShotsEnemies[i].GetX() > WINDOW_WIDTH)	ShotsEnemies[i].ShutDown();
		}
	}
		
	return false;
}
void Game::Draw()
{
	SDL_Rect rc;

	//Set the color used for drawing operations
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	//Clear rendering target
	SDL_RenderClear(Renderer);

	// Draw god mode red wireframe rectangle
	if (god_mode) {
		SDL_SetRenderDrawColor(Renderer, 192, 0, 0, 255);
	}

	//Draw scene
	Scene.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, background_texture, NULL, &rc);
	rc.x += rc.w;
	SDL_RenderCopy(Renderer, background_texture, NULL, &rc);

	//Draw player
	Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, spaceship_texture, NULL, &rc);
	if (god_mode) {
		SDL_RenderDrawRect(Renderer, &rc);
	}


	//Draw enemies
	for (int i = 0; i < 10; i++)
	{
		enemies[i].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
		SDL_RenderCopy(Renderer, enemy_texture, NULL, &rc);
		if (god_mode) {
			SDL_RenderDrawRect(Renderer, &rc);
		}
	}

	//Draw shots
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
			SDL_RenderCopy(Renderer, shot_texture, NULL, &rc);
			if (god_mode) {
				SDL_RenderDrawRect(Renderer, &rc);
			}
		}
	}

	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (ShotsEnemies[i].IsAlive())
		{
			ShotsEnemies[i].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
			SDL_RenderCopy(Renderer, shot_texture, NULL, &rc);
			if (god_mode) {
				SDL_RenderDrawRect(Renderer, &rc);
			}
		}
	}
	//Test enemy





	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
}