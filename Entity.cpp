#include "Entity.h"
#include "SDL/include/SDL_render.h"


Entity::Entity()
{
	is_alive = false;
}
Entity::~Entity()
{
}
void Entity::Init(int posx, int posy, int w, int h, int s)
{
	x = posx;
	y = posy;
	width = w;
	height = h;
	speed = s;
	is_alive = true;
}

void Entity::Init(int posx, int posy, int w, int h, int s, int he)
{
	x = posx;
	y = posy;
	width = w;
	height = h;
	speed = s;
	is_alive = true;
	health = he;
}

void Entity::Init(int posx, int posy, int w, int h, int s, int my, int mx, int he)
{
	x = posx;
	y = posy;
	width = w;
	height = h;
	speed = s;
	movY = my;
	movX = mx;
	is_alive = true;
	health = he;
}

void Entity::GetRect(int* posx, int* posy, int* w, int* h)
{
	*posx = x;
	*posy = y;
	*w = width;
	*h = height;
}
int Entity::GetX()
{
	return x;
}

int Entity::GetY()
{
	return y;
}
int Entity::GetWidth()
{
	return width;
}
int Entity::GetHeight()
{
	return height;
}
void Entity::SetX(int posx)
{
	x = posx;
}
void Entity::SetY(int posy)
{
	y = posy;
}
void Entity::ShutDown()
{
	is_alive = false;
}
bool Entity::IsAlive()
{
	if (this->health <= 0) {
		ShutDown();
	}
	return is_alive;
}

void Entity::SetMovY(int mv) {
	movY = mv;
}

void Entity::SetMovX(int mv) {
	movX = mv;
}

void Entity::Move(int dx, int dy)
{
	x += dx * speed;
	y += dy * speed;
}

void Entity::Move()
{
	y += movY * speed;
	x += movX * speed;
}


void Entity::SetRoll(bool roll) {
	this->roll = roll;
}

bool Entity::GetRoll() {
	return roll;
}

void Entity::SetExitMenu(bool ex) {
	exitMenu = ex;
}

bool Entity::GetExitMenu() {
	return exitMenu;
}

void Entity::SetHeith(int h)
{
	height = h;
}
int Entity::GetHeith()
{
	return height;
}

void Entity::RenderHealthBar(SDL_Renderer* renderer) const {
	SDL_Rect healthBar = { 10, 10, health, 10 };
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &healthBar);
}

void Entity::Damage(int damage) {
	if (this->IsAlive()) {
		this->health -= damage;
	}
}

void Entity::Heal(int heal) {
	if (this->IsAlive()) {
		this->health += heal;
	}
}