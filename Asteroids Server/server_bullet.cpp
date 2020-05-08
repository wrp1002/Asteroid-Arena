#include "server_bullet.h"

server_bullet::server_bullet(float x, float y, float dir, float speed, int life, int type, int ID, int playerID) {
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->speed = speed;
	this->life = life;
	this->type = type;
	this->ID = ID;
	this->playerID = playerID;

	active = true;
	damage = 1;
	radius = 2;

	velX = speed * cos(dir);
	velY = speed * sin(dir);

	if (type == 0) {
		damage = 1;
	}
	else if (type == 1) {
		damage = 0.5;
	}
	else if (type == 2) {
		damage = 0.6;
	}
	else if (type == 3) {
		damage = 0;
		radius = 16;
	}
}

void server_bullet::Update() {
	life--;

	x += velX;
	y -= velY;

	if (x < -radius)
		x = 800 + radius;
	else if (x > 800 + radius)
		x = -radius;
	if (y < -radius)
		y = 600 + radius;
	else if (y > 600 + radius)
		y = -radius;

	life--;
	if (life <= 0)
		Kill();
}

void server_bullet::Kill() {
	active = false;
}

void server_bullet::AddVelocity(float dir, float speed)
{
}

float server_bullet::GetX()
{
	return x;
}

float server_bullet::GetY()
{
	return y;
}

float server_bullet::GetVelX()
{
	return velX;
}

float server_bullet::GetVelY()
{
	return velY;
}

float server_bullet::GetDir()
{
	return dir;
}

float server_bullet::GetSpeed()
{
	return speed;
}

float server_bullet::GetDamage()
{
	return damage;
}

int server_bullet::GetLife()
{
	return life;
}

int server_bullet::GetType()
{
	return type;
}

int server_bullet::GetID()
{
	return ID;
}

int server_bullet::GetShotBy()
{
	return playerID;
}

int server_bullet::GetRadius()
{
	return radius;
}

bool server_bullet::GetActive()
{
	return active;
}
