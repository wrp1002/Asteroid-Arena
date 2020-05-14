#pragma once
#define _USE_MATH_DEFINES
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <enet/enet.h>
#include <math.h>
#include <string>
#include <iostream>
#include <vector>
#include "server_globals.h"
#include "server_player.h"
#include "server_asteroid.h"
#include "server_bullet.h"

using namespace std;

class server_enemy {
private:
	float x, y, velX, velY, dir, moveDir, shootDir, targetDir, health, maxHealth;
	int shootTimer, shootTimerStart, radius, maxVelocity, turnDir;
	int ID;
	bool active;

public:
	server_enemy(int ID, ENetHost* server);

	void Update(vector<server_player>& players, vector<server_asteroid>& asteroids, vector<server_bullet>& bullets, int& bulletIDNum, ENetHost* server);


	int GetID();
	int GetShootTimer();
	int GetRadius();
	float GetX();
	float GetY();
	float GetShootDir();
	float GetTargetDir();
	float GetHealth();
	bool IsActive();

	void SetKnockback(float dir, float speed);
	void AddKnockback(float dir, float speed);
	void ResetShootTimer();
	void SetShootTimer(int s);
	void SetTargetDir(float d);
	void SetShootDir(float d);
	void SetHealth(float h, ENetHost *server);
};

