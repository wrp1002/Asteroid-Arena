#include "client_player.h"


client_player::client_player(int x, int y, int health, string name, ALLEGRO_BITMAP* image) {
	keys[UP] = false;
	keys[DOWN] = false;
	keys[LEFT] = false;
	keys[RIGHT] = false;
	keys[TAB] = false;
	keys[SHOOT] = false;

	this->x = x;
	this->y = y;
	this->health = health;
	this->maxHealth = health;
	this->image = image;
	this->name = name;
	color = 0;

	radius = 8;
	prevX = x;
	prevY = y;
	velX = 0;
	velY = 0;
	boostSpeed = 1;
	boostSpeedMax = 1.5;
	dir = rand() % 361;
	bulletTimer = 0;
	maxHealth = health;
	newHealth = maxHealth;
	invincibleTimer = 0;
	weapon = 0;
	wins = 0;
	turnSpeed = 4;
	accSpeed = 1.75;
	maxSpeed = 12;
	chargeNumMax = 100;
	chargeNum = chargeNumMax;
	moving = false;
	alive = true;
	exploded = false;
	shield = false;
	typing = false;
	ready = true;
	seen = false;
	won = false;
	boost = false;
	boostFull = true;
	ID = -1;
	bulletTimerDefault = 30;
	bulletTimerStart = bulletTimerDefault;
}

client_player::~client_player()
{
}

void client_player::Update(vector<client_particle>& particles, vector<client_effect>& effects, ENetPeer* peer, ALLEGRO_BITMAP* explosionImage, ALLEGRO_BITMAP* flameImage, SoundManager& soundManager) {
	if (alive) {
		prevX = x;
		prevY = y;
		prevDir = dir;

		x += velX;
		y += velY;

		if (bulletTimer > 0)
			bulletTimer--;

		if (keys[SHOOT] && bulletTimer <= 0) {
			bulletTimer = bulletTimerStart;

			char packet[256];
			int packetlen = sprintf_s(packet, sizeof(packet), "Shoot,%i", ID);
			ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, NULL);
			enet_peer_send(peer, 0, p);
		}

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

		if (alive && health <= 0) {
			alive = false;

			for (int i = 0; i < 10; i++)
				effects.push_back(client_effect(x - 32 + rand() % 33 - 16, y - 32 + rand() % 33 - 16, 0, 0, rand() % 5 - 3, 0, explosionImage, soundManager));
			for (int i = 0; i < 20; i++)
				effects.push_back(client_effect(x, y, (rand() % 361 - 180) * (M_PI / 180), 3 + rand() % 3, rand() % 20 + 10, 1, flameImage, soundManager));


			for (int i = 0; i < 5000; i++)
				particles.push_back(client_particle(x, y, 255, rand() % 255, 0, 1, rand() % 30, rand() % 2, rand() % 360 * (M_PI / 180), 3 + rand() % 200 * .01));
		}

		if (moving && health > 0) {
			particles.push_back(client_particle(x - 6 * cos(dir * (M_PI / 180)), y - 6 * sin(dir * (M_PI / 180)), 255, rand() % 255, 0, 1, rand() % 10 + 10, rand() % 3, dir * (M_PI / 180) + (rand() % 91 - 45) * (M_PI / 180) + M_PI, rand() % 100 * .01));
			if (boost)
				effects.push_back(client_effect(x - 4 - 6 * cos(dir * (M_PI / 180)), y - 4 - 6 * sin(dir * (M_PI / 180)), dir * (M_PI / 180) + (rand() % 31 - 15) * (M_PI / 180) + M_PI, rand() % 50 * .01 + 6, rand() % 5 + 5, 1, flameImage, soundManager));
		}

		if (health <= maxHealth * .4) {
			if (rand() % 10 * health == 0) {
				effects.push_back(client_effect(x, y, rand() % 361 * (M_PI / 180), rand() % 10 * .1, rand() % 10 + 10, 1, flameImage, soundManager));
			}
		}
	}
}

void client_player::Draw(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, bool showInfo) {
	if (alive) {
		ALLEGRO_BITMAP* temp = al_create_bitmap(radius * 2, radius * 2);
		al_set_target_bitmap(temp);
		al_draw_bitmap_region(image, moving * radius * 2, radius * 2 * color, radius * 2, radius * 2, 0, 0, NULL);
		al_set_target_bitmap(al_get_backbuffer(display));
		al_draw_rotated_bitmap(temp, radius, radius, x, y, dir * (M_PI / 180), NULL);
		al_destroy_bitmap(temp);
		//al_draw_circle(x, y, radius, al_map_rgb(255, 0, 255), 2);

		if (shield)
			al_draw_circle(x, y, 9, al_map_rgb(0, 50, 255), 2);

		if (won) {
			al_draw_text(font, al_map_rgb(255, 255, 255), x, y - 40, ALLEGRO_ALIGN_CENTER, "WIN");
		}
	}

	if (showInfo)
		al_draw_circle(x, y, radius, al_map_rgb(255, 0, 255), 2);
}

void client_player::SendPacket(ENetPeer* peer) {
	char packet[256];
	int packetlen = sprintf_s(packet, sizeof(packet), "PlayerUpdate,%i,%f,%f,%f,%i,%i", ID, x, y, dir, moving, boost);
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, NULL);
	enet_peer_send(peer, 0, p);
}

void client_player::Win(int wins) {
	won = true;
	this->wins = wins;
}

void client_player::Reset(int screenWidth, int screenHeight) {
	velX = 0;
	velY = 0;
	dir = rand() % 361;
	invincibleTimer = 0;
	x = rand() % screenWidth;
	y = rand() % screenHeight;

	alive = true;
	exploded = false;
	shield = false;
	ready = false;
	won = false;
	boost = false;
	boostFull = true;

	weapon = 0;
	bulletTimerDefault = 30;
	bulletTimerStart = bulletTimerDefault;
	bulletTimer = 0;
	chargeNum = chargeNumMax;
	maxHealth = newHealth;
}

void client_player::ChangeWeapon(int newWeapon) {
	weapon = newWeapon;
	bulletTimer = 0;

	if (weapon == 0) {
		bulletTimerStart = 30;
	}
	else if (weapon == 1) {
		bulletTimerStart = 10;
	}
	else if (weapon == 2) {
		bulletTimerStart = 50;
	}
	else if (weapon == 3) {
		bulletTimerStart = 15;
	}
	else if (weapon == 4) {
		bulletTimerStart = 10;
	}
}

int client_player::GetID() { return ID; }
int client_player::GetWins() { return wins; }
float client_player::GetX() { return x; }
float client_player::GetY() { return y; }
float client_player::GetPrevX() { return prevX; }
float client_player::GetPrevY() { return prevY; }
float client_player::GetDir() { return dir; }
float client_player::GetPrevDir() { return prevDir; }
float client_player::GetHealth() { return health; }
float client_player::GetMaxHealth() { return maxHealth; }
float client_player::GetChargeNum() { return chargeNum; }
float client_player::GetChargeNumMax() { return chargeNumMax; }
bool client_player::GetAlive() { return alive; }
bool client_player::GetMoving() { return moving; }
bool client_player::GetKey(int key) { return keys[key]; }
bool client_player::GetTyping() { return typing; }
bool client_player::GetReady() { return ready; }
bool client_player::GetSeen() { return seen; }
bool client_player::GetShield() { return shield; }
string client_player::GetName() { return name; }
string client_player::GetChatMessage() { return message; }

void client_player::SetVelocity(float speed, float dir) {
	velX = speed * cos(dir);
	velY = speed * sin(dir);
}
void client_player::AddVelocity(float speed, float dir) {
	velX += speed * cos(dir);
	velY -= speed * sin(dir);
}
void client_player::SetReady(bool ready) { this->ready = ready; }
void client_player::SetTyping(bool typing) { this->typing = typing; }
void client_player::SetKey(int key, bool state) { keys[key] = state; }
void client_player::SetSeen(bool seen) { this->seen = seen; }
void client_player::SetName(string name) { this->name = name; }
void client_player::SetChatMessage(string message) { this->message = message; }
void client_player::SetID(int ID) { 
	cout << "Setting ID to " << ID << endl;
	this->ID = ID; 
}
void client_player::SetHealth(float health) { this->health = health; }
void client_player::SetStartingHealth(float health) { this->maxHealth = health; this->health = this->maxHealth; this->newHealth = health; }
void client_player::SetColor(float color) { this->color = color; }
void client_player::SetShield(bool shield) { this->shield = shield; }
void client_player::SetNewHealth(float newHealth) { this->newHealth = newHealth; }