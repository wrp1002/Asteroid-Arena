#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <vector>
#include "client_globals.h"

class client_asteroid {
private:
	float x, y, velX, velY, moveDir, dir, rotDir, rotSpeed, speed;
	int size, radius, ID;
	bool active;
	ALLEGRO_BITMAP *image;

public:
	client_asteroid(int ID, float x, float y, float moveDir, float speed, int size, ALLEGRO_BITMAP* bigAsteroidImage, ALLEGRO_BITMAP* smallAsteroidImage);

	void PacketUpdate(float x, float y, float velX, float velY);
	void Update();
	void Draw(bool showInfo);

	int GetID();
	int GetSize();
	float GetX();
	float GetY();
	bool GetActive();

};


