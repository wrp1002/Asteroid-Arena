#include "server_player.h"

server_player::server_player(int ID, ENetPeer* peer, int color, int startingHealth) {
	this->ID = ID;
	this->peer = peer;
	this->color = color;
	this->startingHealth = startingHealth;
	keys[UP] = false;
	keys[DOWN] = false;
	keys[LEFT] = false;
	keys[RIGHT] = false;
	keys[TAB] = false;
	keys[SHOOT] = false;

	x = 0;
	y = 0;
	velX = 0;
	velY = 0;
	maxSpeed = 12;
	turnSpeed = 4;
	boostSpeed = 1;
	boostSpeedMax = 1.5;
	maxHealth = startingHealth;
	health = maxHealth;
	radius = 8;
	dir = 0;
	wins = 0;
	weapon = 0;
	name = "Null";
	shield = false;
	ready = false;
	authorized = false;
	boost = false;
	boostFull = true;
	chargeNumMax = 100;
	chargeNum = chargeNumMax;
	accSpeed = 1.75;

	authorized = true;
}

void server_player::Init(float x, float y, string name) {
	this->x = x;
	this->y = y;
	this->name = name;
}

void server_player::PacketUpdate(float x, float y, float velX, float velY, float dir, bool up, bool left, bool right, bool boost) {
	this->x = x;
	this->y = y;
	this->velX = velX;
	this->velY = velY;
	this->dir = dir;
	keys[UP] = up;
	keys[LEFT] = left;
	keys[RIGHT] = right;
	keys[BOOST] = boost;
}

void server_player::Update() {
	x += velX;
	y += velY;


	if (keys[UP])
		moving = true;
	else
		moving = false;

	if (keys[LEFT])
		dir -= turnSpeed;
	if (keys[RIGHT])
		dir += turnSpeed;

	if (keys[BOOST] && boostFull && moving) {
		if (!boost) {
			boostSpeed = 1;
			boost = true;
		}
		chargeNum -= 1;
	}
	else if (boost) {
		boost = false;
		boostFull = false;
		boostSpeed = 1;
	}

	if (chargeNum <= 0 && boost) {
		boost = false;
		boostFull = false;
	}

	if (!boost && chargeNum < chargeNumMax) {
		chargeNum += .25;
		if (chargeNum == chargeNumMax)
			boostFull = true;
	}

	if (boost)
		boostSpeed += .125;
	else {
		boostSpeed = 1;
	}

	if (moving) {
		velX += boostSpeed * accSpeed * cos((dir * (M_PI / 180))) / 30;
		velY += boostSpeed * accSpeed * sin((dir * (M_PI / 180))) / 30;
	}
	else {
		if (velX > 0) {
			velX -= .025;
			if (velX < .05)
				velX = 0;
		}
		else if (velX < 0) {
			velX += .025;
			if (velX > -.05)
				velX = 0;
		}
		if (velY > 0) {
			velY -= .025;
			if (velY < .05)
				velY = 0;
		}
		else if (velY < 0) {
			velY += .025;
			if (velY > -.05)
				velY = 0;
		}
	}

	if (velX > maxSpeed)
		velX = maxSpeed;
	else if (velX < -maxSpeed)
		velX = -maxSpeed;
	if (velY > maxSpeed)
		velY = maxSpeed;
	else if (velY < -maxSpeed)
		velY = -maxSpeed;

	if (x < -16)
		x = 816;
	else if (x > 816)
		x = -16;
	if (y < -16)
		y = 616;
	else if (y > 616)
		y = -16;

}

void server_player::Win() {
	wins++;
}

void server_player::SetStartingHealth(float h) {
	startingHealth = h;
}

void server_player::SetHealth(float h, ENetHost* server) {
	health = h;

	char packet[256];
	sprintf_s(packet, sizeof(packet), "PlayerHealth,%i,%f", ID, health);
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, p);
}

void server_player::SetReady(bool ready) {
	this->ready = ready;
}

void server_player::SetAuthorized(bool a) {
	this->authorized = a;
}

void server_player::SetShield(bool s, ENetHost* server) {
	shield = s;

	char packet[256];
	sprintf_s(packet, sizeof(packet), "PlayerShield,%i,%i", ID, shield);
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, p);
}

void server_player::SetWeapon(int w) {
	weapon = w;
}

void server_player::Reset(int startingHealth) {
	keys[UP] = false;
	keys[DOWN] = false;
	keys[LEFT] = false;
	keys[RIGHT] = false;
	keys[TAB] = false;
	keys[SHOOT] = false;

	x = 0;
	y = 0;
	velX = 0;
	velY = 0;
	boostSpeed = 1;
	maxHealth = startingHealth;
	health = maxHealth;
	dir = 0;
	weapon = 0;
	shield = false;
	ready = false;
	boost = false;
	boostFull = true;
	chargeNumMax = 100;
	chargeNum = chargeNumMax;
}

float server_player::GetX()
{
	return x;
}

float server_player::GetY()
{
	return y;
}

int server_player::GetID()
{
	return ID;
}

int server_player::GetColor()
{
	return color;
}

float server_player::GetVelX()
{
	return velX;
}

float server_player::GetVelY()
{
	return velY;
}

float server_player::GetDir()
{
	return dir;
}

float server_player::GetHealth()
{
	return health;
}

float server_player::GetMaxHealth()
{
	return maxHealth;
}

int server_player::GetWins()
{
	return wins;
}

int server_player::GetWeapon()
{
	return weapon;
}

int server_player::GetRadius()
{
	return radius;
}

bool server_player::GetShield()
{
	return shield;
}

bool server_player::GetReady()
{
	return ready;
}

bool server_player::GetAuthorized()
{
	return authorized;
}

string server_player::GetName()
{
	return name;
}

ENetPeer* server_player::GetPeer()
{
	return peer;
}
