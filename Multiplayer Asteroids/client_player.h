#pragma once
#define _USE_MATH_DEFINES
#include <string>
#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <enet/enet.h>
#include <unordered_map>
#include "client_particle.h"
#include "client_effect.h"
#include "client_globals.h"

using namespace std;

class client_player
{
private:
	unordered_map<int, bool> keys;
	int ID, color;
	float x, y, prevX, prevY, velX, velY, dir, prevDir, health, newHealth, maxSpeed;
	float boostSpeed, boostSpeedMax, accSpeed;
	float chargeNumMax, chargeNum;
	int bulletTimerDefault, bulletTimerStart, bulletTimer, maxHealth, radius, invincibleTimerStart, invincibleTimer, weapon, wins, turnSpeed;
	bool moving, alive, exploded, won, shield, typing, ready, seen, boost, boostFull;
	string name, message;
	ALLEGRO_BITMAP* image;







public:
	client_player(int x, int y, int health, string name, ALLEGRO_BITMAP* image);
	~client_player();

	void Update(vector<client_particle>& particles, vector<client_effect>& effects, ENetPeer* peer, ALLEGRO_BITMAP* explosionImage, ALLEGRO_BITMAP* flameImage, SoundManager& soundManager);
	void Draw(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, bool showInfo);

	void ChangeWeapon(int newWeapon);

	void Win(int wins);
	void Reset(int screenWidth, int screenHeight);

	void SendPacket(ENetPeer* peer);

	int GetID();
	int GetWins();
	float GetX();
	float GetY();
	float GetPrevX();
	float GetPrevY();
	float GetDir();
	float GetHealth();
	float GetPrevDir();
	float GetMaxHealth();
	float GetChargeNum();
	float GetChargeNumMax();
	bool GetAlive();
	bool GetMoving();
	bool GetKey(int key);
	bool GetTyping();
	bool GetReady();
	bool GetSeen();
	bool GetShield();
	string GetName();
	string GetChatMessage();

	void SetVelocity(float speed, float dir);
	void AddVelocity(float speed, float dir);
	void SetReady(bool ready);
	void SetTyping(bool typing);
	void SetKey(int key, bool state);
	void SetSeen(bool seen);
	void SetName(string name);
	void SetID(int ID);
	void SetChatMessage(string message);
	void SetHealth(float health);
	void SetStartingHealth(float health);
	void SetColor(float color);
	void SetShield(bool shield);
	void SetNewHealth(float newHealth);
};

