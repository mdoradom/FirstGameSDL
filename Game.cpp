#include "Game.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "SDL_mixer/include/SDL_mixer.h"
#include "ModuleAudio.h"
#pragma comment(lib, "SDL_mixer/libx86/SDL2_mixer.lib")

#define MIX_DEFAULT_FORMAT  AUDIO_S16LSB

Game::Game() {}
Game::~Game(){}
ModuleAudio audio;

bool Game::Init()
{
	//Initialize SDL with all subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
		SDL_Log("SDL_EVENTS could not initializate! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		SDL_Log("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
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
		enemies[i].Init(WINDOW_WIDTH - x * i, -y *i, x, 41, 2, -1, 1, 10);
	}


	//Init variables
	Player.Init(0, WINDOW_HEIGHT >> 1, 104, 82, 5, 100);
	idx_shot = 0;
	idx_shotEnemies = 0;
	int w;
	SDL_QueryTexture(background_texture, NULL, NULL, &w, NULL);
	Scene.Init(0, 0, w, WINDOW_HEIGHT, 4);
	god_mode = false;

	LoadAudios();

	return true;
}

bool Game::LoadAudios() {
	mLaserSound =  audio.LoadFx("assets/laser.wav"); // load laser .wav file
	if (!mLaserSound) {
		SDL_Log(SDL_GetError());
	}
	audio.PlayMusic("assets/stage1.ogg", 3.0F); // play background music
	return true;

}

bool Game::LoadImages() {
	// Load Images
	//Initialize PNG loading
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		SDL_Log("IMG_Init: only PNG support: %s\n", IMG_GetError());
		return false;
	}

	background_texture = SDL_CreateTextureFromSurface(Renderer, IMG_Load("assets/background.png"));
	if (background_texture == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	shot_texture = SDL_CreateTextureFromSurface(Renderer, IMG_Load("assets/shot.png"));
	if (shot_texture == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	spaceship_texture = SDL_CreateTextureFromSurface(Renderer, IMG_Load("assets/spaceship.png"));
	if (spaceship_texture == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	enemy_texture = SDL_CreateTextureFromSurface(Renderer, IMG_Load("assets/spaceship.png"));
	if (enemy_texture == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	//Load image at specified path

	return true;
}

void Game::Release()
{
	// Release laser sound
	audio.UnLoadFx(mLaserSound);

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
	if (keys[SDL_SCANCODE_W] == KEY_REPEAT)	fy = -1;
	if (keys[SDL_SCANCODE_S] == KEY_REPEAT)	fy = 1;
	if (keys[SDL_SCANCODE_A] == KEY_REPEAT)	fx = -1;
	if (keys[SDL_SCANCODE_D] == KEY_REPEAT)	fx = 1;
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN)
	{
		if (Player.IsAlive())
		{
			int x, y, w, h;
			Player.GetRect(&x, &y, &w, &h);
			// shots spawn point are [(29, 3), (29, 59)]
			Shots[idx_shot].Init(x + 29, y + 3, 56, 20, 10, 100);
			Shots[idx_shot + 1].Init(x + 29, y + 59, 56, 20, 10, 10);
			idx_shot += 2;
			idx_shot %= MAX_SHOTS;
			audio.PlayFx(mLaserSound); // play laser sound
		}
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

		if (enemies[i].IsAlive())
		{
			if (shoote < 3) {
				int x, y, w, h;
				enemies[i].GetRect(&x, &y, &w, &h);
				ShotsEnemies[idx_shotEnemies].Init(x - 29, y + 3, 28, 10, 10, 1);
				ShotsEnemies[idx_shotEnemies + 1].Init(x - 29, y + 59, 28, 10, 10, 1);
				idx_shotEnemies += 2;
				idx_shotEnemies %= MAX_SHOTS;
			}
		}
	}

	
	// Scene Scroll
	Scene.Move(-1, 0);
	if (Scene.GetX() <= -Scene.GetWidth())	Scene.SetX(0);

	//Player update
	bool izq = Player.GetX() <= 0;
	bool der = Player.GetX() + Player.GetWidth() >= WINDOW_WIDTH;

	bool arb = Player.GetY() <= 0;
	bool abaj =Player.GetY() + Player.GetHeight() >= WINDOW_HEIGHT;

	if (izq && arb && fx == -1 && fy == -1) {
		fx = fy = 0;
	} else if (izq && abaj && fx == -1 && fy == 1) {
		fx = fy = 0;
	} else if (der && arb && fx == 1 && fy == -1) {
		fx = fy = 0;
	} else if (der && abaj && fx == 1 && fy == 1) {
		fx = fy = 0;
	} else if (izq && fx == -1) {
		fx = 0;
	} else if (der && fx == 1) {
		fx = 0;
	} else if (arb && fy == -1) {
		fy = 0;
	} else if (abaj && fy == 1) {
		fy = 0;
	}

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

	//Enemies death
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		SDL_Rect shotRect = {Shots[i].GetX(), Shots[i].GetY(), Shots[i].GetWidth()-10, Shots[i].GetHeight()-5 };
		for (int j = 0; j < 10; ++j)
		{
			SDL_Rect enemyRect = {enemies[j].GetX(), enemies[j].GetY(), enemies[j].GetWidth(), enemies[j].GetHeight()};

			if (SDL_HasIntersection(&shotRect, &enemyRect) && enemies[j].IsAlive() && Shots[i].IsAlive()) {
				Shots[i].ShutDown();	
				enemies[j].Damage(10);
			}
		}
	}

	//Player death
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		SDL_Rect enemyShotRect = {ShotsEnemies[i].GetX(), ShotsEnemies[i].GetY(), ShotsEnemies[i].GetWidth(), ShotsEnemies[i].GetHeight()};
		SDL_Rect playerRect = {Player.GetX(), Player.GetY(), Player.GetWidth(), Player.GetHeight()};
		if (SDL_HasIntersection(&enemyShotRect, &playerRect) && ShotsEnemies[i].IsAlive()) {
			ShotsEnemies[i].ShutDown();
			if (!god_mode) {
				Player.Damage(5);
			}
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
	if (Player.IsAlive())
	{
		Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
		SDL_RenderCopy(Renderer, spaceship_texture, NULL, &rc);
		if (god_mode) {
			SDL_RenderDrawRect(Renderer, &rc);
		}
	}
	
	//Draw enemies
	for (int i = 0; i < 10; i++)
	{
		if (enemies[i].IsAlive())
		{
			enemies[i].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
			SDL_RenderCopy(Renderer, enemy_texture, NULL, &rc);
			if (god_mode) {
				SDL_RenderDrawRect(Renderer, &rc);
			}
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

	Player.RenderHealthBar(Renderer);

	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
}