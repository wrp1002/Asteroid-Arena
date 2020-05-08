#include "client_other_player.h"


client_other_player::client_other_player(int ID, float x, float y, float dir, string name, float maxHealth, ALLEGRO_BITMAP* image, int color, int wins) {
	this->ID = ID;
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->name = name;
	this->image = image;
	this->maxHealth = maxHealth;
	this->health = maxHealth;
	this->color = color;

	this->wins = wins;
	this->boost = false;
	won = false;
	radius = 8;
	alive = true;
	shield = false;
}

void client_other_player::PacketUpdate(float x, float y, float dir, float health, bool moving, bool boost, bool shield) {
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->health = health;
	this->moving = moving;
	this->boost = boost;
	this->shield = shield;
}

void client_other_player::Update(vector<client_particle>& particles, vector<client_effect>& effects, ALLEGRO_BITMAP* explosionImage, ALLEGRO_BITMAP* flameImage, SoundManager &soundManager) {
	if (moving && health > 0) {
		particles.push_back(client_particle(x - 6 * cos(dir * (M_PI / 180)), y - 6 * sin(dir * (M_PI / 180)), 255, rand() % 255, 0, 1, rand() % 10 + 10, rand() % 3, dir * (M_PI / 180) + (rand() % 61 - 30) * (M_PI / 180) + M_PI, rand() % 100 * .01));
		if (boost)
			effects.push_back(client_effect(x - 4 - 6 * cos(dir * (M_PI / 180)), y - 4 - 6 * sin(dir * (M_PI / 180)), dir * (M_PI / 180) + (rand() % 31 - 15) * (M_PI / 180) + M_PI, rand() % 50 * .01 + 6, rand() % 5 + 5, 1, flameImage, soundManager));
	}

	if (health <= 0 && alive) {
		alive = false;

		for (int i = 0; i < 10; i++)
			effects.push_back(client_effect(x - 32 + rand() % 33 - 16, y - 32 + rand() % 33 - 16, 0, 0, rand() % 5 - 3, 0, explosionImage, soundManager));
		for (int i = 0; i < 20; i++)
			effects.push_back(client_effect(x, y, (rand() % 361 - 180) * (M_PI / 180), 3 + rand() % 3, rand() % 20 + 10, 1, flameImage, soundManager));
		for (int i = 0; i < 5000; i++)
			particles.push_back(client_particle(x, y, 255, rand() % 255, 0, 1, rand() % 30, rand() % 2, rand() % 360 * (M_PI / 180), 3 + rand() % 200 * .01));
	}

	if (alive) {
		if (health <= maxHealth * .4) {
			if (rand() % 10 * health == 0) {
				effects.push_back(client_effect(x, y, rand() % 361 * (M_PI / 180), rand() % 10 * .1, rand() % 10 + 10, 1, flameImage, soundManager));
			}
		}
	}
}

void client_other_player::Draw(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, ALLEGRO_FONT* font2, bool showInfo) {
	if (alive) {
		ALLEGRO_BITMAP* temp = al_create_bitmap(radius * 2, radius * 2);
		al_set_target_bitmap(temp);
		al_draw_bitmap_region(image, moving * radius * 2, radius * 2 * color, radius * 2, radius * 2, 0, 0, NULL);
		al_set_target_bitmap(al_get_backbuffer(display));
		al_draw_rotated_bitmap(temp, radius, radius, x, y, dir * (M_PI / 180), NULL);
		al_destroy_bitmap(temp);

		al_draw_text(font, al_map_rgb(255, 255, 255), x, y - 20, ALLEGRO_ALIGN_CENTER, name.c_str());
		al_draw_filled_rectangle(x - 10, y - 10, x + 10, y - 12, al_map_rgb(255, 0, 0));
		al_draw_filled_rectangle(x - 10, y - 10, x - 10 + 20 * (health / maxHealth), y - 12, al_map_rgb(0, 255, 0));

		if (shield)
			al_draw_circle(x, y, 9, al_map_rgb(0, 50, 255), 2);

		if (won)
			al_draw_text(font2, al_map_rgb(255, 255, 255), x, y - 40, ALLEGRO_ALIGN_CENTER, "WIN");
	}

	if (showInfo)
		al_draw_circle(x, y, radius, al_map_rgb(255, 0, 255), 2);
}

void client_other_player::Win(int wins) {
	won = true;
	this->wins = wins;
}

void client_other_player::Reset() {
	won = false;
	alive = true;
	maxHealth = newHealth;
	health = maxHealth;
}

int client_other_player::GetID() { return ID; }
int client_other_player::GetWins() { return wins; }
float client_other_player::GetX() { return x; }
float client_other_player::GetY() { return y; }
float client_other_player::GetDir() { return dir; }
bool client_other_player::GetAlive() { return alive; }
bool client_other_player::GetMoving() { return moving; }
bool client_other_player::GetShield() { return shield; }
string client_other_player::GetName() { return name; }
void client_other_player::SetNewHealth(float newHealth) { this->newHealth = newHealth; }
void client_other_player::SetStartingHealth(float health) { this->maxHealth = health; this->health = this->maxHealth; this->newHealth = health; }