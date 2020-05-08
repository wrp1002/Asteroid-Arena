#pragma once
#include <math.h>

class server_bullet {
private:
	float x, y, velX, velY, dir, speed, damage;
	int life, type, radius;
	int ID, playerID;
	bool active;

public:
	server_bullet(float x, float y, float dir, float speed, int life, int type, int ID, int playerID);


	void Update();
	void Kill();
	void AddVelocity(float dir, float speed);

	float GetX();
	float GetY();
	float GetVelX();
	float GetVelY();
	float GetDir();
	float GetSpeed();
	float GetDamage();
	int GetLife();
	int GetType();
	int GetID();
	int GetShotBy();
	int GetRadius();
	bool GetActive();
};

