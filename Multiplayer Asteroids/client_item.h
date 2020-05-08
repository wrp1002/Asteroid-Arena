#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>
#include <allegro5\allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5\allegro_primitives.h>

using namespace std;

class client_item {
private:
	float x, y, speed, dir;
	int type, radius, ID;
	bool active;
	
public:
	client_item(int ID, float x, float y, int type, float speed, float dir);
	
	//PacketUpdate(stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stof(msgVars[3].c_str()));
	void PacketUpdate(float x, float y, float dir);
	void Update(int screenWidth, int screenHeight);
	void Draw(ALLEGRO_BITMAP* itemImage, bool showInfo);
	void Kill();

	int GetID();
};

