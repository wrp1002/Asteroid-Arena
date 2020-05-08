#include "client_particle.h"


client_particle::client_particle(float x, float y, int r, int g, int b, int type, int life, float size, float dir, float speed) {
	this->x = x;
	this->y = y;
	this->r = r;
	this->g = g;
	this->b = b;
	this->type = type;
	this->life = life;
	this->size = size;
	this->dir = dir;
	this->speed = speed;
	this->active = true;
}

void client_particle::Update(vector<client_particle>& particles) {
	if (type != 2)
		life--;
	if (life < 0) {
		active = false;
		if (type == 3) {
			if (size > 1.5) {	// OR MAYBE JUST 1
				for (int i = 0; i < 3; i++)
					particles.push_back(client_particle(x, y, r, g, b, type, 100 + rand() % 50, size / 2, rand() % 361 * (M_PI / 180), speed / 2));
			}
		}
	}

	if (type == 0) {
		x += rand() % 3 - 1;
		y += rand() % 3 - 1;
	}
	else if (type == 1) {
		x += speed * cos(dir);
		y += speed * sin(dir);
		if (life < size * 4)
			size -= .25;
	}
	else if (type == 3) {
		x += speed * cos(dir);
		y += speed * sin(dir);
	}
}

void client_particle::Draw() {
	if (size > 0)
		al_draw_filled_circle(x, y, size, al_map_rgb(r, g, b));
	else
		al_draw_pixel(x, y, al_map_rgb(r, g, b));
}

int client_particle::GetActive() { return active; }