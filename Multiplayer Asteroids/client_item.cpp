#include "client_item.h"

client_item::client_item(int ID, float x, float y, int type, float speed, float dir) {
	this->ID = ID;
	this->x = x;
	this->y = y;
	this->type = type;
	this->speed = speed;
	this->dir = dir;
	radius = 8;

	active = true;
}

void client_item::PacketUpdate(float x, float y, float dir) {
	this->x = x;
	this->y = y;
	this->dir = dir;
}

void client_item::Update(int screenWidth, int screenHeight) {
	x += speed * cos(dir);
	y += speed * sin(dir);

	if (x + radius < -radius)
		x = (float)screenWidth + radius;
	else if (x - radius > screenWidth)
		x = -radius;
	if (y + radius < -radius)
		y = (float)screenHeight + radius;
	else if (y - radius > screenHeight)
		y = -radius;
}

void client_item::Draw(ALLEGRO_BITMAP* itemImage, bool showInfo) {
	al_draw_bitmap_region(itemImage, al_get_bitmap_height(itemImage) * type, 0, al_get_bitmap_height(itemImage), al_get_bitmap_height(itemImage), x - al_get_bitmap_height(itemImage) / 2, y - al_get_bitmap_height(itemImage) / 2, NULL);
}

void client_item::Kill() {
	active = false;
}

int client_item::GetID()
{
	return ID;
}
