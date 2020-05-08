#pragma once
#include <enet/enet.h>
#include <string>
#include <iostream>

using namespace std;

class server_player {
private:
	float x, y, dir;
	float startingHealth, health, maxHealth;
	int ID, color, wins, weapon, radius;
	string name;
	ENetPeer *peer;
	bool boost, shield, ready, authorized, moving;

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
	void PacketUpdate(float x, float y, float dir, bool moving, bool boost);
	void Update();

	void Win();

	void SetStartingHealth(float h);
	void SetHealth(float h);
	void SetReady(bool ready);
	void SetAuthorized(bool a);
	void SetShield(bool s);
	void SetWeapon(int w);
	void Reset(int startingHealth);

	
	int GetID();
	int GetColor();
	float GetHealth();
	float GetMaxHealth();
	int GetWins();
	int GetWeapon();
	int GetRadius();
	float GetX();
	float GetY();
	float GetDir();
	bool GetBoost();
	bool GetShield();
	bool GetMoving();
	bool GetReady();
	bool GetAuthorized();
	string GetName();

	ENetPeer *GetPeer();
};

