#pragma once
#include "SDL/include/SDL_render.h"
class Entity
{
public:
	Entity();
	~Entity();
	void Init(int posx, int posy, int w, int h, int s);
	void Init(int posx, int posy, int w, int h, int s, int he);
	void Init(int posx, int posy, int w, int h, int s, int mv, int mx, int he);
	void GetRect(int* posx, int* posy, int* w, int* h);
	void SetX(int posx);
	int  GetX();
	int  GetY();
	void SetMovY(int mv);
	void SetMovX(int mv);
	int  GetWidth();
	int  GetHeight();
	void ShutDown();
	bool IsAlive();
	void Move(int dx, int dy);
	void RenderHealthBar(SDL_Renderer* renderer) const;
	void Damage(int damage);
	void Heal(int heal);

	void Move();

private:
	int x, y;
	int width, height;
	int speed;
	bool is_alive;
	int movY, movX;
	int health;
};

