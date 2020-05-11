#include "client_asteroid.h"


client_asteroid::client_asteroid(int ID, float x, float y, float moveDir, float speed, int size, ALLEGRO_BITMAP* bigAsteroidImage, ALLEGRO_BITMAP* smallAsteroidImage) {
	this->x = x;
	this->y = y;
	this->moveDir = moveDir;
	this->speed = speed;
	this->size = size;
	this->ID = ID;

	velX = speed * cos(moveDir);
	velY = speed * sin(moveDir);

	active = true;
	dir = (rand() % 361 - 180) * (M_PI / 180);

	rotDir = 1;
	if (rand() % 2 == 0)
		rotDir = -1;

	if (size == 0) {
		rotSpeed = (rand() % 400 + 100) * .01;
		this->image = smallAsteroidImage;
		radius = 16;
	}
	else /*if (size == 1)*/ {
		rotSpeed = (rand() % 200 + 100) * .01;
		this->image = bigAsteroidImage;
		radius = 32;
	}
}

void client_asteroid::PacketUpdate(float x, float y, float velX, float velY) {
	this->x = x;
	this->y = y;
	this->velX = velX;
	this->velY = velY;
}

void client_asteroid::Update() {
	dir += rotDir * rotSpeed * (M_PI / 180);

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
		x = (float)SCREEN_WIDTH + radius;
	else if (x - radius > SCREEN_WIDTH)
		x = -radius;
	if (y + radius < -radius)
		y = (float)SCREEN_HEIGHT + radius;
	else if (y - radius > SCREEN_HEIGHT)
		y = -radius;
}

void client_asteroid::Draw(bool showInfo) {
	al_draw_rotated_bitmap(image, radius, radius, x, y, dir, NULL);

	if (showInfo)
		al_draw_circle(x, y, radius, al_map_rgb(255, 0, 255), 2);
}

int client_asteroid::GetID() { return ID; }
int client_asteroid::GetSize() { return size; }
float client_asteroid::GetX() { return x; }
float client_asteroid::GetY() { return y; }
bool client_asteroid::GetActive() { return active; }