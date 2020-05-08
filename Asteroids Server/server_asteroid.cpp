#include "server_asteroid.h"

server_asteroid::server_asteroid(float x, float y, float moveDir, int size, int ID, ENetHost* server) {
	this->x = x;
	this->y = y;
	this->moveDir = moveDir;
	this->size = size;
	this->ID = ID;
	this->server = server;

	active = true;
	dir = moveDir;

	health = 10;
	speed = 2;

	rotDir = 1;
	if (rand() % 2 == 0)
		rotDir = -1;

	if (size == 0) {
		rotSpeed = (rand() % 400 + 100) * .01;
		radius = 16;
		health = 1;		// IDK
		speed = 1;
	}
	else /*if (size == 1)*/ {
		rotSpeed = (rand() % 200 + 100) * .01;
		radius = 32;
		health = 2;	// IDK
		speed = 1;
	}

	velX = speed * cos(dir);
	velY = speed * sin(dir);


	//	asteroids.push_back(client_asteroid(stoi(msgVars[0].c_str()), stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stof(msgVars[3].c_str()), stof(msgVars[4].c_str()), stoi(msgVars[5].c_str()), bigAsteroidImage, smallAsteroidImage));
	//	client_asteroid::client_asteroid(int ID, float x, float y, float moveDir, float speed, int size, ALLEGRO_BITMAP* bigAsteroidImage, ALLEGRO_BITMAP* smallAsteroidImage) 
	char packet[256];
	sprintf_s(packet, sizeof(packet), "NewAsteroid,%i,%f,%f,%f,%f,%i", ID, x, y, moveDir, speed, size);
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, p);
}

void server_asteroid::Update(int screenWidth, int screenHeight) {
	x += velX;
	y += velY;

	if (velX < -1.5)
		velX = -1.5;
	else if (velX > 1.5)
		velX = 1.5;
	if (velY < -1.5)
		velY = -1.5;
	else if (velY > 1.5)
		velY = 1.5;

	if (x + radius < -radius)
		x = (float)screenWidth + radius;
	else if (x - radius > screenWidth)
		x = -radius;
	if (y + radius < -radius)
		y = (float)screenHeight + radius;
	else if (y - radius > screenHeight)
		y = -radius;

	if (health <= 0)
		Kill();
}

void server_asteroid::Kill() {
	active = false;
}

void server_asteroid::AddVelocity(float dir, float speed) {
	velX += speed * cos(dir);
	velY -= speed * sin(dir);
}

void server_asteroid::SetHealth(float h) {
	this->health = h;
}

int server_asteroid::GetHealth() {
	return health;
}

int server_asteroid::GetRadius()
{
	return radius;
}

int server_asteroid::GetID()
{
	return ID;
}

float server_asteroid::GetX()
{
	return x;
}

float server_asteroid::GetY()
{
	return y;
}

float server_asteroid::GetVelX()
{
	return velX;
}

float server_asteroid::GetVelY()
{
	return velY;
}

bool server_asteroid::GetActive()
{
	return active;
}
