#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <vector>
#include <string>
#include "client_particle.h"
#include "SoundManager.h"

using namespace std;

class client_effect {
private:
	float x, y, dir, speed;
	int type, life;
	int frame, frameTimerStart, frameTimer;
	bool active;
	
	ALLEGRO_BITMAP *image;

public:
	client_effect(float x, float y, float dir, float speed, int idk3, int type, ALLEGRO_BITMAP* image, SoundManager& soundManager);

	void Update(vector<client_effect> &effects, vector<client_particle> &particles, ALLEGRO_BITMAP *explosionImage);
	void Draw(bool showInfo);


	int GetType();
	float GetX();
	float GetY();
	float GetDir();
	float GetSpeed();
	bool GetActive();
	
};

