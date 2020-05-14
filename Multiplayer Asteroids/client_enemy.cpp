#include "client_enemy.h"

client_enemy::client_enemy(int ID, float x, float y, float dir, float shootDir, float maxHealth, ALLEGRO_BITMAP* image) {
	this->ID = ID;
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->shootDir = shootDir;
	this->maxHealth = maxHealth;
	this->image = image;

	health = maxHealth;
}

void client_enemy::Update(vector<client_particle> &particles, vector<client_effect> &effects, ALLEGRO_BITMAP *flameImage, SoundManager &soundManager) {
	particles.push_back(client_particle(x + 6 * cos(dir + M_PI), y + 6 * sin(dir + M_PI), 255, rand() % 255, 0, 1, rand() % 10 + 10, rand() % 3, dir + (rand() % 91 - 45) * (M_PI / 180) + M_PI, rand() % 100 * .01));

	if (health <= maxHealth * .5) {
		if (rand() % 10 * health == 0) {
			effects.push_back(client_effect(x, y, rand() % 361 * (M_PI / 180), rand() % 10 * .1, rand() % 10 + 10, 1, flameImage, soundManager));
		}
	}

	if (rand() % 3 == 0)
		soundManager.PlaySample("Move.wav");
}

void client_enemy::PacketUpdate(float x, float y, float dir, float shootDir, float health) {
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->shootDir = shootDir;
	this->health = health;
}

void client_enemy::Draw() {
	al_draw_tinted_scaled_rotated_bitmap_region(image, 0, 0, al_get_bitmap_height(image), al_get_bitmap_height(image), al_map_rgb(255, 255, 255), al_get_bitmap_height(image) / 2, al_get_bitmap_height(image) / 2, x, y, 1, 1, dir, NULL);
	al_draw_tinted_scaled_rotated_bitmap_region(image, al_get_bitmap_height(image), 0, al_get_bitmap_height(image), al_get_bitmap_height(image), al_map_rgb(255, 255, 255), al_get_bitmap_height(image) / 2, al_get_bitmap_height(image) / 2, x, y, 1, 1, shootDir, NULL);

	al_draw_filled_rectangle(x - 10, y - 10, x + 10, y - 12, al_map_rgb(255, 0, 0));
	al_draw_filled_rectangle(x - 10, y - 10, x - 10 + 20 * (health / maxHealth), y - 12, al_map_rgb(0, 255, 0));
}

int client_enemy::GetID()
{
	return ID;
}

float client_enemy::GetX()
{
	return x;
}

float client_enemy::GetY()
{
	return y;
}
