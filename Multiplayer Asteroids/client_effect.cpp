#include "client_effect.h"

/*
BOOST
effects.push_back(client_effect(x - 6 * cos(dir * (M_PI / 180)), y - 6 * sin(dir * (M_PI / 180)), dir * (M_PI / 180) + (rand() % 31 - 15) * (M_PI / 180) + M_PI, rand() % 50 * .01 + 6, rand() % 5 + 5, 1, flameImage));


EXPLOSION
for (int i = 0; i < 10; i++)
	effects.push_back(client_effect(x - 32 + rand() % 33 - 16, y - 32 + rand() % 33 - 16, 0, 0, rand() % 5 - 3, 0, explosionImage));
for (int i = 0; i < 20; i++)
	effects.push_back(client_effect(x, y, (rand() % 361 - 180) * (M_PI / 180), 3 + rand() % 3, rand() % 20 + 10, 1, flameImage));


DURING EXPLOSION
if (rand() % 2 == 0)
	effects.push_back(client_effect(effects[i].GetX() - 32, effects[i].GetY() - 32, 0, 0, rand() % 5 - 3, 0, explosionImage));
if (rand() % 3 == 0)
	effects.push_back(client_effect(effects[i].GetX(), effects[i].GetY(), (rand() % 361 - 180) * (M_PI / 180), 2 + rand() % 2, rand() % 20 - 10, 1, flameImage));
if (rand() % 2 == 0)
	effects.push_back(client_effect(effects[i].GetX(), effects[i].GetY(), (rand() % 361 - 180) * (M_PI / 180), 2, rand() % 20 + 20, 1, flameImage));
*/



client_effect::client_effect(float x, float y, float dir, float speed, int life, int type, ALLEGRO_BITMAP* image, SoundManager& soundManager) {
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->speed = speed;
	this->life = life;
	this->type = type;
	this->image = image;

	frame = 0;
	frameTimerStart = 2;
	frameTimer = frameTimerStart;

	active = true;

	if (type == 0) {
		frame = life;
		if (rand() % 100 == 0)
			soundManager.PlaySample("Explosion" + to_string(1 + rand() % 6) + ".wav");
	}
	else if (type == 1) {
		frameTimer = -1;
	}
}

void client_effect::Update(vector<client_effect>& effects, vector<client_particle>& particles, ALLEGRO_BITMAP* explosionImage) {
	if (type == 0 || type == 2) {
		frameTimer--;
		if (frameTimer <= 0) {
			frame++;
			frameTimer = frameTimerStart;
			if (type == 0) {
				if (frame >= 9)
					active = false;
			}
			else if (type == 2) {
				if (frame >= 6)
					active = false;
			}
		}
	}
	else if (type == 1) {
		//x += rand() % 5 - 2;
		//y += rand() % 5 - 2;
		x += speed * cos(dir);
		y += speed * sin(dir);

		life--;
		if (life <= 0)
			active = false;
	}
}

void client_effect::Draw(bool showInfo) {
	al_draw_bitmap_region(image, al_get_bitmap_height(image) * frame, 0, al_get_bitmap_height(image), al_get_bitmap_height(image), x, y, NULL);
}

int client_effect::GetType()
{
	return type;
}

float client_effect::GetX()
{
	return x;
}

float client_effect::GetY()
{
	return y;
}

float client_effect::GetDir()
{
	return dir;
}

float client_effect::GetSpeed()
{
	return speed;
}

bool client_effect::GetActive()
{
	return active;
}
