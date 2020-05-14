#pragma once
#define _USE_MATH_DEFINES
#include <enet/enet.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <math.h>

using namespace std;

enum KEYS { UP, DOWN, LEFT, RIGHT, TAB, SHOOT, BOOST };

class server_player {
private:
	float x, y, dir, velX, velY, maxSpeed, accSpeed;
	float startingHealth, health, maxHealth;
	float boostSpeed, boostSpeedMax;
	float chargeNum, chargeNumMax;
	int ID, color, wins, weapon, radius, turnSpeed;
	string name;
	ENetPeer *peer;
	bool shield, ready, authorized, moving, boost, boostFull;

	unordered_map<int, bool> keys;


public:
	//	players.push_back(server_player(IDNum, event.peer, color, startingHealth))
	server_player(int ID, ENetPeer *peer, int color, int startingHealth);

	//	player.GetID(), player.GetX(), player.GetY(), player.GetName().c_str()
	void Init(float x, float y, string name);

	//	players[i].PacketUpdate(stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stof(msgVars[3].c_str()), stoi(msgVars[4].c_str()), stoi(msgVars[5].c_str()));
	//	players[i].GetID(), players[i].GetX(), players[i].GetY(), players[i].GetDir(), players[i].GetHealth(), players[i].GetMoving(), players[i].GetBoost(), players[i].GetShield()
	/*
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->health = health;
	this->moving = moving;
	this->boost = boost;
	this->shield = shield;
	*/

	//int packetlen = sprintf_s(packet, sizeof(packet), "PlayerUpdate,%i,%f,%f,%f,%i,%i", ID, x, y, velX, velY, dir, keys[UP], keys[LEFT], keys[RIGHT], keys[BOOST]);
	void PacketUpdate(float x, float y, float velX, float velY, float dir, bool up, bool left, bool right, bool boost);
	void Update();

	void Win();

	void SetStartingHealth(float h);
	void SetHealth(float h, ENetHost* server);
	void SetReady(bool ready);
	void SetAuthorized(bool a);
	void SetShield(bool s, ENetHost* server);
	void SetWeapon(int w);
	void Reset(int startingHealth);

	
	int GetID();
	int GetColor();
	float GetVelX();
	float GetVelY();
	float GetHealth();
	float GetMaxHealth();
	int GetWins();
	int GetWeapon();
	int GetRadius();
	float GetX();
	float GetY();
	float GetDir();
	bool GetShield();
	bool GetReady();
	bool GetAuthorized();
	string GetName();

	ENetPeer *GetPeer();
};

