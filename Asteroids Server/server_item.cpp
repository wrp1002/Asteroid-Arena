#include "server_item.h"

server_item::server_item(float x, float y, int type, int ID, ENetHost* server) {
	this->x = x;
	this->y = y;
	this->type = type;
	this->ID = ID;
	this->server = server;

	active = true;
	dir = (rand() % 360) * (M_PI / 180);
	speed = 3;
	radius = 8;

	//	client_item(stoi(msgVars[0].c_str()), stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stoi(msgVars[3].c_str()), stof(msgVars[4].c_str()), stof(msgVars[5].c_str())));
	char packet[256];
	sprintf_s(packet, sizeof(packet), "NewItem,%i,%f,%f,%i,%f,%f", ID, x, y, type, speed, dir);
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, p);

}

void server_item::Update(int screenWidth, int screenHeight) {
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

void server_item::SendUpdate(ENetHost* server) {
	char packet[256];
	sprintf_s(packet, sizeof(packet), "ItemUpdate,%i,%f,%f,%f", ID, x, y, dir);
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, p);
}

void server_item::SendSound(ENetHost* server) {
	char packet[256];
	sprintf_s(packet, sizeof(packet), "ItemSound");
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, p);
}



void server_item::Kill() {
	active = false;
}

void server_item::SetDir(float d) {
	this->dir = d;
}

int server_item::GetRadius()
{
	return radius;
}

int server_item::GetType()
{
	return type;
}

int server_item::GetID()
{
	return ID;
}

float server_item::GetX()
{
	return x;
}

float server_item::GetY()
{
	return y;
}

bool server_item::GetActive()
{
	return active;
}
