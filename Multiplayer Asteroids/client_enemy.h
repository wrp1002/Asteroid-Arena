#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <vector>
#include "client_particle.h"
#include "client_effect.h"
#include "SoundManager.h"

using namespace std;

class client_enemy {
private:
	float x, y, dir, shootDir, health, maxHealth;
	int ID;
	ALLEGRO_BITMAP* image;

public:
	client_enemy(int iD, float x, float y, float dir, float shootDir, float maxHealth, ALLEGRO_BITMAP* image);

	void Update(vector<client_particle>& particles, vector<client_effect>& effects, ALLEGRO_BITMAP* flameImage, SoundManager& soundManager);
	void PacketUpdate(float x, float y, float dir, float shootDir, float health);
	void Draw();

	int GetID();
	float GetX();
	float GetY();
};


