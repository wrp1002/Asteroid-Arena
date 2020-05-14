#pragma once
#define _USE_MATH_DEFINES
#include <enet/enet.h>
#include <string>
#include <math.h>

using namespace std;

class server_item {
private:
	float x, y, dir, speed;
	int ID, type, radius;
	int updateTimerStart, updateTimer;
	bool active;
	ENetHost *server;

public:
	//	items.push_back(server_item(x, y, rand() % 6, itemIDNum, server));
	server_item(float x, float y, int type, int ID, ENetHost *server);

	void Update(int screenWidth, int screenHeight);
	void SendUpdate(ENetHost *server);
	void SendSound(ENetHost* server);
	void Kill();

	void SetDir(float d);

	int GetRadius();
	int GetType();
	int GetID();
	float GetX();
	float GetY();
	bool GetActive();

};

