#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <vector>

using namespace std;

class client_particle
{
private:
	float x, y, dir, speed, size;
	int life, r, g, b, type;
	bool active;
public:
	client_particle(float x, float y, int r, int g, int b, int type, int life, float size, float dir, float speed);

	void Update(vector<client_particle>& particles);
	void Draw();

	int GetActive();
};

