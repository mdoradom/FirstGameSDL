#pragma once

#include "SDL/include/SDL.h"
#include "SDL_image/include/SDL_image.h"
#pragma comment(lib, "SDL_image/libx86/SDL2_image.lib")
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

#include "Entity.h"
#include <list>
using namespace std;
#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768
#define MAX_KEYS		256
#define MAX_SHOTS		320
typedef unsigned int uint;


class Game
{
public:
	Game();
	~Game();

	bool Init();
	bool LoadImages();
	bool LoadAudios();
	void Release();
	bool DisplayMenu();
	void SpawnEnemies();
	bool Input();
	bool Update();
	void Draw();

private:
	SDL_Texture* background_texture, * spaceship_texture, * shot_texture, * enemy_texture, * boss_texture, * menu_texture;

	SDL_Window *Window;
	SDL_Renderer *Renderer;


	Entity Player, Shots[MAX_SHOTS], Scene, enemies[10], ShotsEnemies[MAX_SHOTS], Boss, ShotsBoss[MAX_SHOTS];


	int idx_shot, idx_shotEnemies, idx_shotBoss, inc, round;

	bool god_mode;

	enum KEY_STATE { KEY_IDLE, KEY_DOWN, KEY_REPEAT, KEY_UP	};
	KEY_STATE keys[MAX_KEYS]; 

	uint mLaserSound, mEnemyLaserSound, mDeathSound, mBossLaserSound, mBossDeathSound, mPlayerHitted;
};
