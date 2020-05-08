#pragma once
#define _USE_MATH_DEFINES
#include <string>
#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_primitives.h>
#include <cmath>
#include <vector>
#include <iostream>
#include "client_particle.h"
#include "client_effect.h"

using namespace std;

class client_other_player
{
private:
	int ID, radius, wins, color;
	float x, y, dir, health, maxHealth, newHealth;
	bool alive, moving, won, boost, shield;
	string name;
	ALLEGRO_BITMAP* image;

public:
	client_other_player(int ID, float x, float y, float dir, string name, float maxHealth, ALLEGRO_BITMAP* image, int color, int wins);

	void PacketUpdate(float x, float y, float dir, float health, bool moving, bool boost, bool shield);
	void Update(vector<client_particle>& particles, vector<client_effect>& effects, ALLEGRO_BITMAP* explosionImage, ALLEGRO_BITMAP* flameImage, SoundManager& soundManager);
	void Draw(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, ALLEGRO_FONT* font2, bool showInfo);
	void Win(int wins);
	void Reset();

	int GetID();
	int GetWins();
	float GetX();
	float GetY();
	float GetDir();
	bool GetAlive();
	bool GetMoving();
	bool GetShield();
	string GetName();
	void SetNewHealth(float newHealth);
	void SetStartingHealth(float health);
};

