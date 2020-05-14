#include "client_bullet.h"


client_bullet::client_bullet(float x, float y, float dir, int type, float speed, int ID, ALLEGRO_BITMAP* image) {
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->type = type;
	this->speed = speed;
	this->image = image;
	this->ID = ID;
	this->velX = speed * cos(dir);
	this->velY = speed * sin(dir);

	frame = 0;
	imageSize = al_get_bitmap_height(image);

	frameTimerStart = -1;
	frameTimer = frameTimerStart;
	frameDir = 1;
	radius = 2;

	cout << "New Bullet! Type:" << type << endl;

	if (type == 0) {
		frameTimerStart = 3;
		frameTimer = frameTimerStart;
		radius = 2;
		
	}
	else if (type == 1) {
		
	}
	else if (type == 2) {
		
	}
	else if (type == 3) {
		frameTimerStart = 4;
		frameTimer = frameTimerStart;
		frameDir = 1;
		radius = 16;
	}
}

void client_bullet::PacketUpdate(float x, float y, float velX, float velY) {
	this->x = x;
	this->y = y;
	this->velX =  velX;		//	HMMMMM......
	this->velY = velY;
}

void client_bullet::Update(vector<client_particle>& particles) {
	x += velX;
	y -= velY;

	if (x < -radius)
		x = 800 + radius;
	else if (x > 800 + radius)
		x = -radius;
	if (y < -radius)
		y = 600 + radius;
	else if (y > 600 + radius)
		y = -radius;

	frameTimer--;
	if (frameTimer == 0) {
		frameTimer = frameTimerStart;
		if (type == 0) {
			frame++;
			if (frame > 4)
				frame = 0;
		}
		else if (type == 3) {
			frame += frameDir;
			if (frame > 3 || frame < 0)
				frameDir *= -1;
		}
	}

	if (rand() % 3 == 0 && type == 0) {
		//int r, int g, int b, int type, int life, int size, float dir, float speed
		//0, 142, 92, 0, 40, 1, 0, 0);
		particles.push_back(client_particle(x + rand() % 5 - 2, y + rand() % 5 - 2, 0, 142, 92, 0, 40, 0, 0, 0));
	}
}

void client_bullet::Draw(ALLEGRO_DISPLAY* display, bool showInfo) {
	ALLEGRO_BITMAP* temp = al_create_bitmap(imageSize, imageSize);
	al_set_target_bitmap(temp);
	al_draw_bitmap_region(image, frame * imageSize, 0, imageSize, imageSize, 0, 0, NULL);
	al_set_target_bitmap(al_get_backbuffer(display));
	al_draw_rotated_bitmap(temp, imageSize / 2, imageSize / 2, x, y, -dir, NULL);
	al_destroy_bitmap(temp);

	if (showInfo)
		al_draw_circle(x, y, radius, al_map_rgb(255, 0, 255), 2);
}

float client_bullet::GetX() { return x; }
float client_bullet::GetY() { return y; }
int client_bullet::GetImageSize() { return imageSize; }
int client_bullet::GetID() { return ID; }

int client_bullet::GetType()
{
	return type;
}
