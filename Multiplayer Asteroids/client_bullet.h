#pragma once
#include <string>
#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "client_particle.h"

using namespace std;

class client_bullet {
private:
	float x, y, velX, velY, dir, speed;
	int type, radius, ID;
	ALLEGRO_BITMAP *image;
	int imageSize, frame, frameTimer, frameTimerStart, frameDir;

public:
	client_bullet(float x, float y, float dir, int type, int speed, int ID, ALLEGRO_BITMAP* image);

	void PacketUpdate(float x, float y, float velX, float velY);
	void Update(vector<client_particle>& particles);
	void Draw(ALLEGRO_DISPLAY* display, bool showInfo);

	float GetX();
	float GetY();
	int GetImageSize();
	int GetID();
	int GetType();
};