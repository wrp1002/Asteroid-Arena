#pragma once
#define _USE_MATH_DEFINES
#include <enet/enet.h>
#include <math.h>
#include <string>

class server_asteroid {
private:
	float x, y, velX, velY, moveDir, dir, rotDir, rotSpeed, speed, health;
	int size, radius, ID;
	bool active;
	ENetHost *server;


public:
	//	asteroids.push_back(server_asteroid(x, y, moveDir, size, asteroidIDNum, server));
	server_asteroid(float x, float y, float moveDir, int size, int ID, ENetHost *server);

	void Update(int screenWidth, int screenHeight);
	void Kill();

	void AddVelocity(float dir, float speed);
	void SetHealth(float h);

	int GetHealth();
	int GetRadius();
	int GetID();
	float GetX();
	float GetY();
	float GetVelX();
	float GetVelY();
	bool GetActive();

};

