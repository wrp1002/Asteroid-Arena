#include "server_player.h"

server_player::server_player(int ID, ENetPeer* peer, int color, int startingHealth) {
	this->ID = ID;
	this->peer = peer;
	this->color = color;
	this->startingHealth = startingHealth;

	x = 0;
	y = 0;
	maxHealth = startingHealth;
	health = maxHealth;
	radius = 8;
	dir = 0;
	wins = 0;
	weapon = 0;
	name = "Null";
	shield = false;
	boost = false;
	ready = false;
	authorized = false;

	cout << "constdructotr health: " << health << endl;
}

void server_player::Init(float x, float y, string name) {
	this->x = x;
	this->y = y;
	this->name = name;
}

void server_player::PacketUpdate(float x, float y, float dir, bool moving, bool boost) {
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->moving = moving;
	this->boost = boost;
}

void server_player::Update() {
	// Do this later
}

void server_player::Win() {
	wins++;
}

void server_player::SetStartingHealth(float h) {
	startingHealth = h;
}

void server_player::SetHealth(float h) {
	health = h;
}

void server_player::SetReady(bool ready) {
	this->ready = ready;
}

void server_player::SetAuthorized(bool a) {
	this->authorized = a;
}

void server_player::SetShield(bool s) {
	shield = s;
}

void server_player::SetWeapon(int w) {
	weapon = w;
}

void server_player::Reset(int startingHealth) {
	this->startingHealth = startingHealth;

	x = 0;
	y = 0;
	maxHealth = startingHealth;
	health = maxHealth;
	radius = 8;
	dir = 0;
	weapon = 0;
	boost = false;
	ready = false;
	//authorized = false;
}

float server_player::GetX()
{
	return x;
}

float server_player::GetY()
{
	return y;
}

int server_player::GetID()
{
	return ID;
}

int server_player::GetColor()
{
	return color;
}

float server_player::GetDir()
{
	return dir;
}

float server_player::GetHealth()
{
	return health;
}

float server_player::GetMaxHealth()
{
	return maxHealth;
}

int server_player::GetWins()
{
	return wins;
}

int server_player::GetWeapon()
{
	return weapon;
}

int server_player::GetRadius()
{
	return radius;
}

bool server_player::GetBoost()
{
	return boost;
}

bool server_player::GetShield()
{
	return shield;
}

bool server_player::GetMoving()
{
	return moving;
}

bool server_player::GetReady()
{
	return ready;
}

bool server_player::GetAuthorized()
{
	return authorized;
}

string server_player::GetName()
{
	return name;
}

ENetPeer* server_player::GetPeer()
{
	return peer;
}
