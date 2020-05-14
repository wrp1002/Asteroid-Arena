#include "server_enemy.h"

server_enemy::server_enemy(int ID, ENetHost *server) {
	this->ID = ID;
	x = 0;
	y = 0;
	velX = 0;
	velY = 0;
	active = true;
	shootTimerStart = 10;
	shootTimer = shootTimerStart;
	maxVelocity = 1;
	turnDir = (rand() % 360) * (M_PI / 180);
	dir = (rand() % 360) * (M_PI / 180);
	moveDir = (rand() % 360) * (M_PI / 180);
	shootDir = (rand() % 360) * (M_PI / 180);
	targetDir = (rand() % 360) * (M_PI / 180);
	radius = 8;


	maxHealth = 10;
	health = maxHealth;

	int place = rand() % 4;
	if (place == 0) {
		x = rand() % SCREEN_WIDTH;
		y = -(rand() % 30 + 50);
	}
	else if (place == 1) {
		x = SCREEN_WIDTH + (rand() % 30 + 50);
		y = rand() % SCREEN_HEIGHT;
	}
	else if (place == 2) {
		x = rand() % SCREEN_WIDTH;
		y = SCREEN_HEIGHT + (rand() % 30 + 50);
	}
	else if (place == 3) {
		x = -(rand() % 30 + 50);
		y = rand() % SCREEN_HEIGHT;
	}

	char packet[256];
	sprintf_s(packet, sizeof(packet), "NewEnemy,%i,%f,%f,%f,%f,%f", ID, x, y, dir, shootDir, maxHealth);
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, p);
}

void server_enemy::Update(vector<server_player>& players, vector<server_asteroid>& asteroids, vector<server_bullet> &bullets, int &bulletIDNum, ENetHost* server) {
	if (active) {
		if (health <= 0)
			active = false;

		x += velX;
		y += velY;

		velX += 1 * cos(dir) / 30;
		velY += 1 * sin(dir) / 30;

		if (turnDir == 0 && rand() % 30 == 0) {
			if (rand() % 2 == 0)
				turnDir = -1;
			else
				turnDir = 1;
		}
		if (turnDir != 0 && rand() % 20 == 0)
			turnDir = 0;

		//cout << turnDir << endl;

		dir += ((rand() % 200) * .01) * (M_PI / 180) * turnDir;

		if (shootTimer > 0)
			shootTimer--;

		if (x < -16)
			x = SCREEN_WIDTH;
		else if (x > SCREEN_WIDTH)
			x = -16;

		if (y < -16)
			y = SCREEN_HEIGHT;
		else if (y > SCREEN_HEIGHT)
			y = -16;

		if (velX > maxVelocity)
			velX = maxVelocity;
		else if (velX < -maxVelocity)
			velX = -maxVelocity;
		if (velY > maxVelocity)
			velY = maxVelocity;
		else if (velY < -maxVelocity)
			velY = -maxVelocity;

		//if (rand() % 3 == 0)
		//	NewParticle(particles, x + 8 + 8 * cos((dir * (PI / 180)) + PI), y + 8 + 8 * sin((dir * (PI / 180)) + PI), 255, rand() % 255, 0, 1, rand() % 10 + 10, rand() % 2 + 1, dir * (PI / 180) + (rand() % 61 - 30) * (PI / 180) + PI, rand() % 100 * .01);


		/*
		if (health <= 0) {
			active = SDL_FALSE;
			int i, j;
			for (j = 0; j < 10; j++)
				NewBullet(bullets, x + 8 + rand() % 33 - 16, y + 8 + rand() % 33 - 16, -1, 0, 0, rand() % 5 - 3, 1);
			for (j = 0; j < 20; j++)
				NewBullet(bullets, x + 8, y + 8, -1, (rand() % 361 - 180) * (PI / 180), 3, rand() % 20 + 10, 2);

			for (i = 0; i < maxItems; i++) {
				if (!items[i].active) {
					items[i].x = x;
					items[i].y = y;
					items[i].dir = (rand() % 361) * (PI / 180);
					items[i].active = SDL_TRUE;
					items[i].speed = rand() % 2 + 1;
					items[i].life = 2000;
					items[i].type = 6;
					break;
				}
			}
		}
		*/

		float distance = 100000;

		for (int j = 0; j < players.size(); j++) {
			float distance2 = GetDistance(GetX() + 8, GetY() + 8, players[j].GetX() + 8, players[j].GetY() + 8);
			if (distance2 < distance) {
				distance = distance2;
				SetTargetDir(GetAngle(players[j].GetX() + 8, players[j].GetY() + 8, GetX() + 8, GetY() + 8));
			}
		}

		
		for (int j = 0; j < asteroids.size(); j++) {
			if (asteroids[j].GetActive()) {
				float distance2 = GetDistance(GetX(), GetY(), asteroids[j].GetX(), asteroids[j].GetY());
				if (distance2 < distance) {
					distance = distance2;
					SetTargetDir(GetAngle(asteroids[j].GetX(), asteroids[j].GetY(), GetX(), GetY()));
				}
			}
		}
		

		float rotateAmount = 3 * (M_PI / 180);

		if (GetDistance(GetX() + 5 * cos((GetShootDir() + rotateAmount)), GetY() + 5 * sin((GetShootDir() + rotateAmount)), GetX() + 5 * cos(GetTargetDir()), GetY() + 5 * sin(GetTargetDir())) < GetDistance(GetX() + 5 * cos(GetShootDir()), GetY() + 5 * sin(GetShootDir()), GetX() + 5 * cos(GetTargetDir()), GetY() + 5 * sin(GetTargetDir())))
			SetShootDir(GetShootDir() + rotateAmount);
		else
			SetShootDir(GetShootDir() - rotateAmount);



		/*if (shootDir > 360)
			shootDir = 0;
		else if (shootDir < 0)
			shootDir = 360;
			*/


		if (distance < 70 && GetDistance(GetX() + 5 * cos(GetShootDir()), GetY() + 5 * sin(GetShootDir()), GetX() + 5 * cos(GetTargetDir()), GetY() + 5 * sin(GetTargetDir())) < 5) {
			SetShootTimer(GetShootTimer() - 1);

			if (GetShootTimer() <= 0) {
				ResetShootTimer();

				for (float k = -.25; k <= .25; k += .25) {
					int amount = 5;
					float fireDir = -shootDir + k;
					if (k == 0)
						amount = 8;

					int speed = 4;
					float x = this->x + amount * cos(fireDir);
					float y = this->y + amount * sin(fireDir);

					bullets.push_back(server_bullet(x, y, fireDir, speed, 25, 2, bulletIDNum, -1));

					char packet[256];
					sprintf_s(packet, sizeof(packet), "NewBullet,%f,%f,%f,%i,%i,%i", x, y, fireDir, speed, 2, bulletIDNum);
					ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(server, 0, p);

					bulletIDNum++;
				}
			}
		}
		else
			ResetShootTimer();

		char packet[256];
		sprintf_s(packet, sizeof(packet), "EnemyUpdate,%i,%f,%f,%f,%f,%f", ID, x, y, dir, shootDir, health);
		ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(server, 0, p);
	}
}

int server_enemy::GetID()
{
	return ID;
}

int server_enemy::GetShootTimer()
{
	return shootTimer;
}

int server_enemy::GetRadius()
{
	return radius;
}

void server_enemy::SetKnockback(float dir, float speed) {
	velX = speed * cos(dir);
	velY = speed * sin(dir);
}

void server_enemy::AddKnockback(float dir, float speed) {
	velX += speed * cos(dir);
	velY -= speed * sin(dir);
}

void server_enemy::ResetShootTimer()
{
	shootTimer = shootTimerStart;
}

float server_enemy::GetX()
{
	return x;
}

float server_enemy::GetY()
{
	return y;
}

float server_enemy::GetShootDir()
{
	return shootDir;
}

float server_enemy::GetTargetDir()
{
	return targetDir;
}

float server_enemy::GetHealth()
{
	return health;
}

bool server_enemy::IsActive()
{
	return active;
}

void server_enemy::SetShootTimer(int s) {
	shootTimer = s;
}

void server_enemy::SetTargetDir(float d) {
	targetDir = d;
}

void server_enemy::SetShootDir(float d) {
	shootDir = d;
}

void server_enemy::SetHealth(float h, ENetHost* server) {
	if (h < health) {
		char packet[256];
		sprintf_s(packet, sizeof(packet), "EnemyHurt");
		ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(server, 0, p);
	}

	health = h;
}


