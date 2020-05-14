#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <enet\enet.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <allegro5\allegro.h>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "server_globals.h"
#include "server_player.h"
#include "server_bullet.h"
#include "server_asteroid.h"
#include "server_item.h"
#include "server_enemy.h"

using namespace std;

const int screenWidth = 800, screenHeight = 600;
bool restartGame = true;

void UpdateGame(int& gameState, bool& countDown, int& startTimer, int startTimerStart, vector<server_player>& players, vector<server_bullet>& bullets, vector<server_asteroid>& asteroids, vector<server_item>& items, vector<server_enemy>& enemies, ENetHost* server, bool& resetCount, int resetTimerStart, int& resetTimer, int startingHealth);
void Collide(vector<server_player>& players, vector<server_bullet>& bullets, vector<server_asteroid>& asteroids, vector<server_item>& items, vector<server_enemy>& enemies, int& enemyIDNum, ENetHost* server);
void CleanUp(vector<server_bullet>& bullets, vector<server_asteroid>& asteroids, vector<server_item>& items, vector<server_enemy> &enemies, ENetHost* server, int& asteroidIDNum, int& itemIDNum);
void ResetGame(int& gameState, bool& countDown, int& startTimer, int startTimerStart, vector<server_player>& players, vector<server_bullet>& bullets, vector<server_asteroid>& asteroids, vector<server_item>& items, vector<server_enemy> &enemies, ENetHost* server, bool& resetCount, int resetTimerStart, int& resetTimer, int startingHealth);
void SpawnAsteroids(vector<server_asteroid>& asteroids, ENetHost* server, int& asteroidIDNum);
void NewAsteroid(vector<server_asteroid>& asteroids, int x, int y, int moveDir, int size, int& asteroidIDNum, ENetHost* server);
void SpawnItems(vector<server_item>& items, int& itemSpawnTimer, int& itemIDNum, ENetHost* server);

void SendChat(string message, int r, int g, int b, ENetHost* server);
void MessagePlayer(string message, int r, int g, int b, ENetHost* server, int playerID);

void LoadConfig(int& maxPlayers, int& startingHealth, int &port);
void SaveConfig(int maxPlayers, int startingHealth);

void ClearScreen();
string generatePassword();
bool IsNumber(string n);

int main() {
	while (restartGame) {
		srand(time(0));
		bool done = false;
		bool shutdown = false;
		int gameState = 0;
		int resetTimerStart = 500;
		int resetTimer = resetTimerStart;
		bool resetCount = false;
		int startTimerStart = 205;
		int startTimer = startTimerStart;
		bool countDown = false;
		int playerUpdateTimerStart = 20;
		int playerUpdateTimer = playerUpdateTimerStart;
		int itemSpawnTimer = rand() % 300 + 500;
		string tempInput = "";
		string input = "";
		int IDNum = 0;
		int bulletIDNum = 0;
		int asteroidIDNum = 0;
		int itemIDNum = 0;
		int enemyIDNum = 0;
		int maxPlayers = 4;
		int startingHealth = 5;
		string password = generatePassword();
		cout << "Password: " << password << endl;

		int port = 12345;

		LoadConfig(maxPlayers, startingHealth, port);

		vector<server_player> players;
		vector<server_bullet> bullets;
		vector<server_asteroid> asteroids;
		vector<server_item> items;
		vector<server_enemy> enemies;

		system("title Shooter Server");

		al_init();
		if (!enet_initialize())
			cout << "Initialized ENet\n";

		ENetAddress address;
		ENetHost* server;
		ENetEvent event;

		address.host = ENET_HOST_ANY;
		address.port = port;
		server = enet_host_create(&address, 32, 1, 0, 0);

		if (address.port != NULL)
			cout << "Binded to port " << address.port << endl;
		if (server != NULL)
			cout << "Successfully created ENet Server\n";

		if (server == NULL) {
			cout << "ERROR: Could not create ENet Server " << endl << "Press any key to exit...\n";
			system("pause>nul");
			exit(EXIT_FAILURE);
		}

		ALLEGRO_EVENT_QUEUE* event_queue = NULL;
		ALLEGRO_TIMER* timer = NULL;

		event_queue = al_create_event_queue();
		timer = al_create_timer(1.0 / 60.0);

		al_register_event_source(event_queue, al_get_timer_event_source(timer));

		al_start_timer(timer);

		while (!done) {
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);

			if (ev.type == ALLEGRO_EVENT_TIMER) {
				UpdateGame(gameState, countDown, startTimer, startTimerStart, players, bullets, asteroids, items, enemies, server, resetCount, resetTimerStart, resetTimer, startingHealth);

				for (auto& bullet : bullets)
					bullet.Update();
				for (auto& asteroid : asteroids)
					asteroid.Update(screenWidth, screenHeight);
				for (auto& item : items)
					item.Update(screenWidth, screenHeight);
				for (auto& player : players)
					player.Update();
				for (auto& enemy : enemies)
					enemy.Update(players, asteroids, bullets, bulletIDNum, server);

				if (gameState == 1 && !resetCount) {
					Collide(players, bullets, asteroids, items, enemies, enemyIDNum, server);
					if (asteroids.size() < 10 && rand() % 60 == 0)
						SpawnAsteroids(asteroids, server, asteroidIDNum);
					SpawnItems(items, itemSpawnTimer, itemIDNum, server);

					/*if (rand() % 50000 == 0) {
						enemies.push_back(server_enemy(enemyIDNum, server));
						enemyIDNum++;
					}*/
				}

				CleanUp(bullets, asteroids, items, enemies, server, asteroidIDNum, itemIDNum);

				playerUpdateTimer--;
				if (playerUpdateTimer <= 0) {
					playerUpdateTimer = playerUpdateTimerStart;
					for (unsigned int i = 0; i < players.size(); i++) {
						/*char packet[256];
						sprintf_s(packet, sizeof(packet), "PlayerUpdate,%i,%f,%f,%f,%f,%i,%i,%i", players[i].GetID(), players[i].GetX(), players[i].GetY(), players[i].GetDir(), players[i].GetHealth(), players[i].GetMoving(), players[i].GetBoost(), players[i].GetShield());
						ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
						enet_host_broadcast(server, 0, p);*/
					}
				}
				if (shutdown && players.size() == 0)
					done = true;
			}

			while (enet_host_service(server, &event, 0) > 0) {
				switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
				{
					if (players.size() < (unsigned)maxPlayers && gameState == 0) {
						printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
						//event.peer->data = "Client";

						int color = 0;
						bool goodColor = false;
						while (!goodColor) {
							color = rand() % 6;
							goodColor = true;
							for (unsigned i = 0; i < players.size(); i++) {
								if (players[i].GetColor() == color) {
									goodColor = false;
									break;
								}
							}
						}

						char packet[256];
						int packetlen = sprintf_s(packet, sizeof(packet), "PlayerID,%i,%i", IDNum, color);
						ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
						printf("Sent a packet to client containing %s\n", packet);
						printf("New Clients ID: %i\n", IDNum);
						enet_peer_send(event.peer, 0, p);

						packetlen = sprintf_s(packet, sizeof(packet), "MaxHealth,%i", startingHealth);
						p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
						enet_host_broadcast(server, 0, p);

						players.push_back(server_player(IDNum, event.peer, color, startingHealth));
						IDNum++;

						Beep(400, 150);
						Beep(300, 150);

						cout << "Size: " << players.size() << endl;
					}
					else {
						if (gameState != 0) {
							char packet[256] = "Kick,Game Running";
							ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(event.peer, 0, p);
						}
						else if (players.size() >= (unsigned)maxPlayers) {
							char packet[256] = "Kick,Server Full";
							ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(event.peer, 0, p);
						}
					}
				}
				break;

				case ENET_EVENT_TYPE_RECEIVE:
				{
					printf("Recieved a packet containing %s\n", event.packet->data);
					int messageCount = 0;
					string Type;
					char message[100];
					vector<string> msgVars;

					for (unsigned int i = 0; i < event.packet->dataLength; ++i) {
						message[i] = event.packet->data[i];
					}
					string coords = message;
					istringstream ss(coords);
					string token;

					while (getline(ss, token, ',')) {
						if (messageCount == 0)
							Type = token;
						else
							msgVars.push_back(token);
						messageCount++;
					}

					if (Type == "PlayerUpdate") {
						int ID = stoi(msgVars[0].c_str());

						for (unsigned int i = 0; i < players.size(); i++) {
							if (players[i].GetID() == ID) {
								players[i].PacketUpdate(stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stof(msgVars[3].c_str()), stof(msgVars[4].c_str()), stof(msgVars[5].c_str()), stoi(msgVars[6].c_str()), stoi(msgVars[7].c_str()), stoi(msgVars[8].c_str()), stoi(msgVars[9].c_str()));

								char packet[256];
								sprintf_s(packet, sizeof(packet), "PlayerUpdate,%i,%f,%f,%f,%f,%f,%i,%i,%i,%i", stoi(msgVars[0].c_str()), stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stof(msgVars[3].c_str()), stof(msgVars[4].c_str()), stof(msgVars[5].c_str()), stoi(msgVars[6].c_str()), stoi(msgVars[7].c_str()), stoi(msgVars[8].c_str()), stoi(msgVars[9].c_str()));
								ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, NULL);
								enet_host_broadcast(server, 0, p);
								//cout << "ID:" << players[i].ID << " X:" << players[i].x << " Y:" << players[i].y << " Charging:" << players[i].charging << endl;
								break;
							}
						}
					}
					else if (Type == "PlayerInfo") {
						int ID = ID = stoi(msgVars[0].c_str());
						string verify = "";

						for (unsigned int i = 0; i < players.size(); i++) {
							if (players[i].GetID() == ID) {
								players[i].Init(stof(msgVars[1].c_str()), stoi(msgVars[2].c_str()), msgVars[3]);

								cout << "Name: " << players[i].GetName() << " ID: " << players[i].GetID() << " X: " << players[i].GetX() << " Y: " << players[i].GetY() << endl << endl;
								for (unsigned int j = 0; j < players.size(); j++) {
									char packet[256];
									sprintf_s(packet, sizeof(packet), "NewPlayer,%i,%f,%f,%f,%s,%f,%i,%i", players[j].GetID(), players[j].GetX(), players[j].GetY(), players[j].GetDir(), players[j].GetName().c_str(), players[j].GetMaxHealth(), players[j].GetColor(), players[j].GetWins());
									ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
									enet_host_broadcast(server, 0, p);
								}

								string message = players[i].GetName() + " has joined the game";
								SendChat(message, 255, 150, 0, server);
								break;
							}
						}
					}
					else if (Type == "PlayerDisconnect") {
						int ID = stoi(msgVars[0].c_str());
						for (unsigned int i = 0; i < players.size(); i++) {
							if (players[i].GetID() == ID) {
								enet_peer_disconnect(players[i].GetPeer(), 0);
								break;
							}
						}
					}
					else if (Type == "Ready") {
						int ID = stoi(msgVars[0].c_str());
						for (unsigned int i = 0; i < players.size(); i++) {
							if (players[i].GetID() == ID) {
								players[i].SetReady(true);
								string message = players[i].GetName() + " is ready";
								SendChat(message, 255, 150, 0, server);
							}
						}

					}
					else if (Type == "Chat") {
						string message = msgVars[0];
						transform(message.begin(), message.end(), message.begin(), tolower);
						int start = message.find_first_of(": ");
						string playerName = message.substr(0, start);
						message.erase(0, start + 2);

						if (message[0] == '/') {
							message.erase(message.begin());
							vector<string> commandVars;
							istringstream ss(message);
							string token;

							while (getline(ss, token, ' '))
								commandVars.push_back(token);


							for (unsigned j = 0; j < players.size(); j++) {
								string testName = players[j].GetName();
								transform(testName.begin(), testName.end(), testName.begin(), tolower);
								if (testName == playerName) {
									if (players[j].GetAuthorized() || commandVars[0] == "password" || commandVars[0] == "help") {
										if (commandVars[0] == "password") {
											if (commandVars[1] == password) {
												players[j].SetAuthorized(true);
												MessagePlayer("You are now authorized to use commands", 0, 255, 0, server, players[j].GetID());
											}
											else
												MessagePlayer("Password Incorrect", 255, 0, 0, server, players[j].GetID());
										}
										else if (commandVars[0] == "help") {
											vector<string> commands;
											commands.push_back("Commands:");
											commands.push_back("/help - Display Commands");
											commands.push_back("/password <password>");
											commands.push_back("/kick <Player Name>");
											commands.push_back("/stop - Stop Server");
											commands.push_back("/restart - Restart Server");
											commands.push_back("/new - Restart Game");
											commands.push_back("/setHealth <Starting Health>");
											commands.push_back("/asteroids - Spawn Asteroids");
											commands.push_back("/items - Spawn Items");
											commands.push_back("/weapon - <Weapon ID>");
											commands.push_back("/enemy - Spawn enemy");

											for (unsigned i = 0; i < commands.size(); i++)
												MessagePlayer(commands[i], 255, 255, 255, server, players[j].GetID());
										}
										else if (commandVars[0] == "kick") {
											if (commandVars.size() != 2) {
												MessagePlayer("Invalid syntax", 255, 0, 0, server, players[j].GetID());
												break;
											}

											for (unsigned i = 0; i < players.size(); i++) {
												string name = players[i].GetName();
												transform(name.begin(), name.end(), name.begin(), tolower);

												if (name == commandVars[1]) {
													char packet[256] = "Kick,Kicked From Game";
													ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
													enet_peer_send(players[i].GetPeer(), 0, p);
													cout << "Kicked " << name << endl;
													break;
												}
											}
										}
										else if (commandVars[0] == "restart") {
											system("cls");
											shutdown = true;
											char packet[256] = "Kick,Server Restarting";
											ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
											enet_host_broadcast(server, 0, p);
										}
										else if (commandVars[0] == "new") {
											resetCount = true;
											SendChat("Restarting Game...", 255, 0, 0, server);
										}
										else if (commandVars[0] == "sethealth") {
											if (commandVars.size() != 2 || !IsNumber(commandVars[1])) {
												MessagePlayer("Invalid syntax", 255, 0, 0, server, players[j].GetID());
												break;
											}

											startingHealth = stoi(commandVars[1].c_str());
											SaveConfig(maxPlayers, startingHealth);
											string healthMessage = "Starting Health Set To " + commandVars[1];

											SendChat(healthMessage, 255, 150, 0, server);

											char packet[256];
											sprintf_s(packet, sizeof(packet), "StartingHealth,%i", startingHealth);
											ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
											enet_host_broadcast(server, 0, p);

											if (gameState == 0 && startTimer == startTimerStart) {
												for (unsigned i = 0; i < players.size(); i++) {
													//cout << "Setting health to " << startingHealth << endl;
													players[i].SetStartingHealth(startingHealth);
													players[i].SetHealth(startingHealth, server);
												}
											}
										}
										else if (commandVars[0] == "stop") {
											shutdown = true;
											restartGame = false;
											char packet[256] = "Kick,Server Shutting Down";
											ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
											enet_host_broadcast(server, 0, p);
										}
										else if (commandVars[0] == "asteroids") {
											for (int i = 0; i < 20; i++)
												SpawnAsteroids(asteroids, server, asteroidIDNum);
										}
										else if (commandVars[0] == "items") {
											for (int i = 0; i < 20; i++) {
												itemSpawnTimer = 0;
												SpawnItems(items, itemSpawnTimer, itemIDNum, server);
											}
										}
										else if (commandVars[0] == "enemy") {
											enemies.push_back(server_enemy(enemyIDNum, server));
											enemyIDNum++;

											MessagePlayer("Spawned enemy!", 255, 255, 255, server, players[j].GetID());
										}
										else if (commandVars[0] == "weapon") {
											if (commandVars.size() != 2 || !IsNumber(commandVars[1])) {
												MessagePlayer("Invalid syntax", 255, 0, 0, server, players[j].GetID());
												break;
											}


											int weapon = stoi(commandVars[1].c_str());

											players[j].SetWeapon(weapon);
											MessagePlayer("Set weapon to " + to_string(weapon), 255, 255, 255, server, players[j].GetID());

											char packet[256];
											sprintf_s(packet, sizeof(packet), "Weapon,%i,%i", players[j].GetID(), players[j].GetWeapon());
											ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
											enet_host_broadcast(server, 0, p);
										}
										else
											MessagePlayer("Unrecognized Command", 255, 0, 0, server, players[j].GetID());
									}
									else
										MessagePlayer("You are not authorized to use commands", 255, 0, 0, server, players[j].GetID());

									break;
								}
							}
						}
						else {
							char message2[256];
							sprintf_s(message2, 256, "%s", msgVars[0].c_str());
							SendChat(message2, 255, 255, 255, server);
						}
					}
					else if (Type == "Shoot") {
						int ID = stoi(msgVars[0].c_str());
						for (unsigned int i = 0; i < players.size(); i++) {
							if (players[i].GetID() == ID) {
								if (players[i].GetWeapon() == 0) {
									
									float x = players[i].GetX() + 5 * cos(players[i].GetDir() * (M_PI / 180));
									float y = players[i].GetY() + 5 * sin(players[i].GetDir() * (M_PI / 180));

									float speed = 5;

									float playerVelX = players[i].GetVelX();
									float playerVelY = players[i].GetVelY();

									float bulletDir = -players[i].GetDir() * (M_PI / 180);
									float bulletVelX = speed * cos(bulletDir);
									float bulletVelY = speed * sin(bulletDir);

									bulletVelX += playerVelX;
									bulletVelY -= playerVelY;																		

									float bulletSpeed = sqrt(bulletVelX * bulletVelX + bulletVelY * bulletVelY);


									bullets.push_back(server_bullet(x, y, bulletDir, bulletSpeed, 40, players[i].GetWeapon(), bulletIDNum, players[i].GetID()));

									char packet[256];
									sprintf_s(packet, sizeof(packet), "NewBullet,%f,%f,%f,%f,%i,%i", x, y, bulletDir, bulletSpeed, players[i].GetWeapon(), bulletIDNum);
									ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
									enet_host_broadcast(server, 0, p);
								}
								else if (players[i].GetWeapon() == 1) {
									float x = players[i].GetX() + 5 * cos(players[i].GetDir() * (M_PI / 180));
									float y = players[i].GetY() + 5 * sin(players[i].GetDir() * (M_PI / 180));

									float speed = 5;

									float playerVelX = players[i].GetVelX();
									float playerVelY = players[i].GetVelY();

									float bulletDir = -players[i].GetDir() * (M_PI / 180);
									float bulletVelX = speed * cos(bulletDir);
									float bulletVelY = speed * sin(bulletDir);

									bulletVelX += playerVelX;
									bulletVelY -= playerVelY;

									float bulletSpeed = sqrt(bulletVelX * bulletVelX + bulletVelY * bulletVelY);

									bullets.push_back(server_bullet(x, y, bulletDir, bulletSpeed, 40, players[i].GetWeapon(), bulletIDNum, players[i].GetID()));

									char packet[256];
									sprintf_s(packet, sizeof(packet), "NewBullet,%f,%f,%f,%f,%i,%i", x, y, bulletDir, bulletSpeed, players[i].GetWeapon(), bulletIDNum);
									ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
									enet_host_broadcast(server, 0, p);
								}
								else if (players[i].GetWeapon() == 2 || players[i].GetWeapon() == 4) {
									for (float k = -.25; k <= .25; k += .25) {
										int amount = 5;
										float bulletDir = -players[i].GetDir() * (M_PI / 180) + k;
										if (k == 0)
											amount = 8;
										
										float x = players[i].GetX() + amount * cos(players[i].GetDir() * (M_PI / 180));
										float y = players[i].GetY() + amount * sin(players[i].GetDir() * (M_PI / 180));

										float speed = 5;

										float playerVelX = players[i].GetVelX();
										float playerVelY = players[i].GetVelY();

										float bulletVelX = speed * cos(bulletDir);
										float bulletVelY = speed * sin(bulletDir);

										bulletVelX += playerVelX;
										bulletVelY -= playerVelY;

										float bulletSpeed = sqrt(bulletVelX * bulletVelX + bulletVelY * bulletVelY);


										bullets.push_back(server_bullet(x, y, bulletDir, bulletSpeed, 40, players[i].GetWeapon(), bulletIDNum, players[i].GetID()));

										char packet[256];
										sprintf_s(packet, sizeof(packet), "NewBullet,%f,%f,%f,%f,%i,%i", x, y, bulletDir, bulletSpeed, players[i].GetWeapon(), bulletIDNum);
										ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
										enet_host_broadcast(server, 0, p);

										bulletIDNum++;
									}
								}
								else if (players[i].GetWeapon() == 3) {
									float x = players[i].GetX() + 5 * cos(players[i].GetDir() * (M_PI / 180));
									float y = players[i].GetY() + 5 * sin(players[i].GetDir() * (M_PI / 180));

									float speed = 5;

									float playerVelX = players[i].GetVelX();
									float playerVelY = players[i].GetVelY();

									float bulletDir = -players[i].GetDir() * (M_PI / 180);
									float bulletVelX = speed * cos(bulletDir);
									float bulletVelY = speed * sin(bulletDir);

									bulletVelX += playerVelX;
									bulletVelY -= playerVelY;

									float bulletSpeed = sqrt(bulletVelX * bulletVelX + bulletVelY * bulletVelY);

									bullets.push_back(server_bullet(x, y, bulletDir, bulletSpeed, 40, players[i].GetWeapon(), bulletIDNum, players[i].GetID()));

									char packet[256];
									sprintf_s(packet, sizeof(packet), "NewBullet,%f,%f,%f,%f,%i,%i", x, y, bulletDir, bulletSpeed, players[i].GetWeapon(), bulletIDNum);
									ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
									enet_host_broadcast(server, 0, p);
								}

								bulletIDNum++;
								break;
							}
						}

					}
					enet_packet_destroy(event.packet);
				}
				break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("%s disconected.\n", event.peer->data);
					event.peer->data = NULL;
					for (unsigned int i = 0; i < players.size(); i++) {
						if (players[i].GetPeer()->address.host == event.peer->address.host && players[i].GetPeer()->address.port == event.peer->address.port) {
							cout << "Size: " << players.size() << endl;
							char packet[256];
							sprintf_s(packet, sizeof(packet), "ErasePlayer,%i", players[i].GetID());
							ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
							enet_host_broadcast(server, 0, p);

							string message = players[i].GetName() + " has left the game";
							SendChat(message, 255, 150, 0, server);

							players.erase(players.begin() + i);
							break;
						}
					}
					if (players.size() == 0) {
						done = true;
						system("cls");
						cout << "Restarted Server\n\n";
					}
					break;
				}
			}

		}
		enet_host_destroy(server);
	}
}

void UpdateGame(int& gameState, bool& countDown, int& startTimer, int startTimerStart, vector<server_player>& players, vector<server_bullet>& bullets, vector<server_asteroid>& asteroids, vector<server_item>& items, vector<server_enemy> &enemies, ENetHost* server, bool& resetCount, int resetTimerStart, int& resetTimer, int startingHealth) {
	if (gameState == 0 && players.size() > 0 && countDown == false) {
		int readyNum = 0;

		for (unsigned int i = 0; i < players.size(); i++) {
			if (players[i].GetReady())
				readyNum++;
		}
		if (readyNum == players.size()) {
			countDown = true;
			cout << "All players ready...\n";
		}
	}
	if (countDown && !resetCount) {
		startTimer--;
		if (startTimer <= 0) {
			countDown = false;
			gameState = 1;
			startTimer = startTimerStart;
			cout << "Game started...\n";

			char packet[256];
			sprintf_s(packet, sizeof(packet), "StartTimer,%i", startTimer);
			ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(server, 0, p);
		}

		if (startTimer % 50 == 0) {
			char packet[256];
			sprintf_s(packet, sizeof(packet), "StartTimer,%i", startTimer);
			ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(server, 0, p);
		}
	}
	if (resetCount) {
		resetTimer--;
		if (resetTimer <= 0) {
			char packet[256] = "ResetGame";
			ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(server, 0, p);

			ResetGame(gameState, countDown, startTimer, startTimerStart, players, bullets, asteroids, items, enemies, server, resetCount, resetTimerStart, resetTimer, startingHealth);
			cout << "Resetting game...\n";
		}
	}

	if (players.size() > 1 || gameState == 1) {
		int liveNum = 0;
		int winID = -1;
		int wins = 0;
		for (unsigned int i = 0; i < players.size(); i++) {
			if (players[i].GetHealth() > 0) {
				liveNum++;
				winID = players[i].GetID();
			}
		}
		if (((liveNum == 1 && players.size() > 1) || (liveNum == 0 && players.size() == 1)) && resetCount == false) {
			cout << "All players dead...\n";
			resetCount = true;

			for (unsigned i = 0; i < players.size(); i++) {
				if (players[i].GetID() == winID) {
					players[i].Win();
					wins = players[i].GetWins();
					break;
				}
			}

			char packet[256];
			sprintf_s(packet, sizeof(packet), "Winner,%i,%i", winID, wins);
			ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(server, 0, p);
		}
	}
}

void Collide(vector<server_player>& players, vector<server_bullet>& bullets, vector<server_asteroid>& asteroids, vector<server_item>& items, vector<server_enemy> &enemies, int &enemyIDNum, ENetHost* server) {
	for (unsigned i = 0; i < players.size(); i++) {
		for (unsigned j = 0; j < players.size(); j++) {
			if (players[i].GetID() != players[j].GetID() && players[i].GetHealth() > 0 && players[j].GetHealth() > 0) {
				if (GetDistance(players[i].GetX(), players[i].GetY(), players[j].GetX(), players[j].GetY()) < 16) {
					char packet[256];
					sprintf_s(packet, sizeof(packet), "SetKnockback,%i,%f,%f", players[i].GetID(), 1.5, GetAngle(players[i].GetX(), players[i].GetY(), players[j].GetX(), players[j].GetY()));
					ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(server, 0, p);
				}
			}
		}
		for (unsigned j = 0; j < bullets.size(); j++) {
			if (bullets[j].GetType() != 10) {
				if (GetDistance(players[i].GetX(), players[i].GetY(), bullets[j].GetX(), bullets[j].GetY()) < players[i].GetRadius() + bullets[j].GetRadius() && bullets[j].GetShotBy() != players[i].GetID()) {
					if (!players[i].GetShield()) {
						if (bullets[j].GetActive())
							players[i].SetHealth(players[i].GetHealth() - bullets[j].GetDamage(), server);

						if (bullets[j].GetType() != 3) {
							char packet[256];
							sprintf_s(packet, sizeof(packet), "AddKnockback,%i,%f,%f", players[i].GetID(), 2.5, bullets[j].GetDir());
							ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
							enet_host_broadcast(server, 0, p);
						}
						else {
							char packet[256];
							sprintf_s(packet, sizeof(packet), "AddKnockback,%i,%f,%f", players[i].GetID(), .25, bullets[j].GetDir());
							ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
							enet_host_broadcast(server, 0, p);
						}
					}
					else {
						players[i].SetShield(false, server);
						bullets.push_back(server_bullet(players[i].GetX(), players[i].GetY(), 0, 0, 5, 10, -1, -1));
					}
					if (bullets[j].GetType() != 3)
						bullets[j].Kill();
				}
			}
		}
		for (unsigned j = 0; j < asteroids.size(); j++) {
			if (GetDistance(players[i].GetX(), players[i].GetY(), asteroids[j].GetX(), asteroids[j].GetY()) < players[i].GetRadius() + asteroids[j].GetRadius()) {
				if (!players[i].GetShield()) {
					players[i].SetHealth(players[i].GetHealth() - 1, server);
					char packet[256];
					sprintf_s(packet, sizeof(packet), "SetKnockback,%i,%f,%f", players[i].GetID(), 3.0, GetAngle(players[i].GetX(), players[i].GetY(), asteroids[j].GetX(), asteroids[j].GetY()));
					ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(server, 0, p);
				}
				else {
					players[i].SetShield(false, server);
					bullets.push_back(server_bullet(players[i].GetX(), players[i].GetY(), 0, 0, 5, 10, -1, -1));
				}

				asteroids[j].Kill();
			}
		}
	}

	for (unsigned i = 0; i < bullets.size(); i++) {
		if (bullets[i].GetType() != 10) {
			for (unsigned j = 0; j < asteroids.size(); j++) {
				if (GetDistance(bullets[i].GetX(), bullets[i].GetY(), asteroids[j].GetX(), asteroids[j].GetY()) < bullets[i].GetRadius() + asteroids[j].GetRadius()) {
					asteroids[j].SetHealth(asteroids[j].GetHealth() - bullets[i].GetDamage());

					if (bullets[i].GetType() != 3) {
						asteroids[j].AddVelocity(bullets[i].GetDir(), 1);

						char packet[256];
						sprintf_s(packet, sizeof(packet), "AsteroidUpdate,%i,%f,%f,%f,%f,%i", asteroids[j].GetID(), asteroids[j].GetX(), asteroids[j].GetY(), asteroids[j].GetVelX(), asteroids[j].GetVelY(), 1);
						ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
						enet_host_broadcast(server, 0, p);

						bullets[i].Kill();
					}
					else {
						asteroids[j].AddVelocity(bullets[i].GetDir(), .05);

						char packet[256];
						sprintf_s(packet, sizeof(packet), "AsteroidUpdate,%i,%f,%f,%f,%f,%i", asteroids[j].GetID(), asteroids[j].GetX(), asteroids[j].GetY(), asteroids[j].GetVelX(), asteroids[j].GetVelY(), 0);
						ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
						enet_host_broadcast(server, 0, p);
					}
				}
			}
			if (bullets[i].GetType() == 3) {
				for (unsigned j = 0; j < bullets.size(); j++) {
					if (i != j && bullets[j].GetType() != 3) {
						if (GetDistance(bullets[i].GetX(), bullets[i].GetY(), bullets[j].GetX(), bullets[j].GetY()) < bullets[i].GetRadius() + bullets[j].GetRadius()) {
							bullets[j].AddVelocity(bullets[i].GetDir(), .5);
							char packet[256];
							sprintf_s(packet, sizeof(packet), "BulletUpdate,%i,%f,%f,%f,%f", bullets[j].GetID(), bullets[j].GetX(), bullets[j].GetY(), bullets[j].GetVelX(), bullets[j].GetVelY());
							ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
							enet_host_broadcast(server, 0, p);
						}
					}
				}
			}
		}
	}

	for (unsigned i = 0; i < items.size(); i++) {
		for (unsigned j = 0; j < asteroids.size(); j++) {
			if (GetDistance(items[i].GetX(), items[i].GetY(), asteroids[j].GetX(), asteroids[j].GetY()) < items[i].GetRadius() + asteroids[j].GetRadius()) {
				items[i].SetDir(GetAngle(items[i].GetX(), items[i].GetY(), asteroids[j].GetX(), asteroids[j].GetY()));
				items[i].SendUpdate(server);
			}
		}
		for (unsigned j = 0; j < bullets.size(); j++) {
			if (bullets[j].GetType() != 10) {
				if (GetDistance(items[i].GetX(), items[i].GetY(), bullets[j].GetX(), bullets[j].GetY()) < items[i].GetRadius() + bullets[j].GetRadius()) {
					items[i].SetDir(-bullets[j].GetDir());
					items[i].SendUpdate(server);
					if (bullets[j].GetType() != 3)
						bullets[j].Kill();
				}
			}
		}
		for (unsigned j = 0; j < players.size(); j++) {
			if (GetDistance(items[i].GetX(), items[i].GetY(), players[j].GetX(), players[j].GetY()) < items[i].GetRadius() + players[j].GetRadius()) {
				if (items[i].GetType() == 0) {
					players[j].SetHealth(players[j].GetHealth() + 2, server);
				}
				else if (items[i].GetType() == 1) {
					players[j].SetShield(true, server);
				}
				else if (items[i].GetType() == 2) {
					players[j].SetWeapon(1);

					char packet[256];
					sprintf_s(packet, sizeof(packet), "Weapon,%i,%i", players[j].GetID(), players[j].GetWeapon());
					ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(server, 0, p);
				}
				else if (items[i].GetType() == 3) {
					players[j].SetWeapon(2);

					char packet[256];
					sprintf_s(packet, sizeof(packet), "Weapon,%i,%i", players[j].GetID(), players[j].GetWeapon());
					ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(server, 0, p);
				}
				else if (items[i].GetType() == 4) {
					players[j].SetWeapon(3);

					char packet[256];
					sprintf_s(packet, sizeof(packet), "Weapon,%i,%i", players[j].GetID(), players[j].GetWeapon());
					ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(server, 0, p);
				}
				else if (items[i].GetType() == 5) {
					players[j].SetWeapon(0);

					char packet[256];
					sprintf_s(packet, sizeof(packet), "Weapon,%i,%i", players[j].GetID(), players[j].GetWeapon());
					ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(server, 0, p);
				}
				else if (items[i].GetType() == 6) {
					players[j].SetWeapon(4);

					char packet[256];
					sprintf_s(packet, sizeof(packet), "Weapon,%i,%i", players[j].GetID(), players[j].GetWeapon());
					ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(server, 0, p);
				}
				else if (items[i].GetType() == 7) {
					enemies.push_back(server_enemy(enemyIDNum, server));
					enemyIDNum++;
				}

				items[i].SendSound(server);
				items[i].Kill();
			}
		}
	}


	for (int i = 0; i < enemies.size(); i++) {
		for (unsigned j = 0; j < players.size(); j++) {
			if (GetDistance(enemies[i].GetX(), enemies[i].GetY(), players[j].GetX(), players[j].GetY()) < 16) {
				enemies[i].SetKnockback(GetAngle(enemies[i].GetX(), enemies[i].GetY(), players[j].GetX(), players[j].GetY()), 1.5);
			}
		}
		for (unsigned j = 0; j < bullets.size(); j++) {
			if (bullets[j].GetType() != 10) {
				if (GetDistance(enemies[i].GetX(), enemies[i].GetY(), bullets[j].GetX(), bullets[j].GetY()) < enemies[i].GetRadius() + bullets[j].GetRadius() && bullets[j].GetShotBy() != -1) {
					if (bullets[j].GetActive())
						enemies[i].SetHealth(enemies[i].GetHealth() - bullets[j].GetDamage(), server);

					if (bullets[j].GetType() != 3)
						enemies[i].AddKnockback(bullets[j].GetDir(), 3.5);
					else
						enemies[i].AddKnockback(bullets[j].GetDir(), 0.25);

					if (bullets[j].GetType() != 3)
						bullets[j].Kill();
				}
			}
		}
		for (unsigned j = 0; j < asteroids.size(); j++) {
			if (GetDistance(enemies[i].GetX(), enemies[i].GetY(), asteroids[j].GetX(), asteroids[j].GetY()) < enemies[i].GetRadius() + asteroids[j].GetRadius()) {
				enemies[i].SetHealth(enemies[i].GetHealth() - 1, server);
				asteroids[j].Kill();
			}
		}
			
	}
}

void ResetGame(int& gameState, bool& countDown, int& startTimer, int startTimerStart, vector<server_player>& players, vector<server_bullet>& bullets, vector<server_asteroid>& asteroids, vector<server_item>& items, vector<server_enemy> &enemies, ENetHost* server, bool& resetCount, int resetTimerStart, int& resetTimer, int startingHealth) {
	for (auto& player : players) {
		player.Reset(startingHealth);
	}

	gameState = 0;

	countDown = false;
	startTimer = startTimerStart;
	resetCount = false;
	resetTimer = resetTimerStart;

	bullets.clear();
	asteroids.clear();
	items.clear();
	enemies.clear();
}

void SpawnAsteroids(vector<server_asteroid>& asteroids, ENetHost* server, int& asteroidIDNum) {
	int x = 0, y = 0;
	int place = rand() % 4;

	if (place == 0) {
		x = rand() % screenWidth;
		y = -640;
	}
	else if (place == 1) {
		x = screenWidth + 640;
		y = rand() % screenHeight;
	}
	else if (place == 2) {
		x = rand() % screenWidth;
		y = screenHeight + 640;
	}
	else if (place == 3) {
		x = -640;
		y = rand() % screenHeight;
	}

	NewAsteroid(asteroids, x, y, (rand() % 361 - 180) * (M_PI / 180), rand() % 2, asteroidIDNum, server);
}

void NewAsteroid(vector<server_asteroid>& asteroids, int x, int y, int moveDir, int size, int& asteroidIDNum, ENetHost* server) {
	asteroids.push_back(server_asteroid(x, y, moveDir, size, asteroidIDNum, server));
	asteroidIDNum++;
}

void SpawnItems(vector<server_item>& items, int& itemSpawnTimer, int& itemIDNum, ENetHost* server) {
	itemSpawnTimer--;
	if (itemSpawnTimer <= 0) {
		itemSpawnTimer = rand() % 300 + 500;

		int place = rand() % 4;
		int x, y;
		int type = rand() % 7;
		if (type == 6) {
			if (rand() % 5 == 0)
				type = 7;
			else
				return;
		}

		if (place == 0) {
			x = rand() % screenWidth;
			y = -(rand() % 30 + 64);
		}
		else if (place == 1) {
			x = screenWidth + (rand() % 30 + 64);
			y = rand() % screenHeight;
		}
		else if (place == 2) {
			x = rand() % screenWidth;
			y = screenHeight + (rand() % 30 + 64);
		}
		else if (place == 3) {
			x = -(rand() % 30 + 64);
			y = rand() % screenHeight;
		}
		items.push_back(server_item(x, y, type, itemIDNum, server));
		itemIDNum++;
	}
}

void CleanUp(vector<server_bullet>& bullets, vector<server_asteroid>& asteroids, vector<server_item>& items, vector<server_enemy> &enemies, ENetHost* server, int& asteroidIDNum, int &itemIDNum) {
	for (unsigned i = 0; i < bullets.size();) {
		if (bullets[i].GetActive())
			i++;
		else {
			if (bullets[i].GetType() == 10) {
				for (unsigned k = 0; k < asteroids.size(); k++) {
					if (GetDistance(bullets[i].GetX(), bullets[i].GetY(), asteroids[k].GetX(), asteroids[k].GetY()) < 128 + asteroids[k].GetRadius()) {
						asteroids[k].AddVelocity(-GetAngle(asteroids[k].GetX(), asteroids[k].GetY(), bullets[i].GetX(), bullets[i].GetY()), 1);
						char packet[256];
						sprintf_s(packet, sizeof(packet), "AsteroidUpdate,%i,%f,%f,%f,%f,%i", asteroids[k].GetID(), asteroids[k].GetX(), asteroids[k].GetY(), asteroids[k].GetVelX(), asteroids[k].GetVelY(), 0);
						ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
						enet_host_broadcast(server, 0, p);
					}
				}
			}

			char packet[256];
			sprintf_s(packet, sizeof(packet), "EraseBullet,%i", bullets[i].GetID());
			ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(server, 0, p);

			bullets.erase(bullets.begin() + i);
		}
	}

	for (unsigned i = 0; i < asteroids.size();) {
		if (asteroids[i].GetActive())
			i++;
		else {
			if (asteroids[i].GetRadius() == 32) {
				float dir = (rand() % 360) * (M_PI / 180);
				int amount = rand() % 2 + 3;
				for (int j = 0; j < amount; j++) {
					asteroids.push_back(server_asteroid(asteroids[i].GetX(), asteroids[i].GetY(), dir, 0, asteroidIDNum, server));
					dir += M_PI / (amount - 1);
					asteroidIDNum++;
				}
			}

			char packet[256];
			sprintf_s(packet, sizeof(packet), "EraseAsteroid,%i", asteroids[i].GetID());
			ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(server, 0, p);

			asteroids.erase(asteroids.begin() + i);
		}
	}

	for (unsigned i = 0; i < items.size();) {
		if (items[i].GetActive())
			i++;
		else {
			char packet[256];
			sprintf_s(packet, sizeof(packet), "EraseItem,%i", items[i].GetID());
			ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(server, 0, p);

			items.erase(items.begin() + i);
		}
	}

	for (unsigned i = 0; i < enemies.size();) {
		if (enemies[i].IsActive())
			i++;
		else {
			char packet[256];
			sprintf_s(packet, sizeof(packet), "EraseEnemy,%i", enemies[i].GetID());
			ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(server, 0, p);

			enemies.erase(enemies.begin() + i);

				

				int place = rand() % 4;
				int x, y;
				if (place == 0) {
					x = rand() % screenWidth;
					y = -(rand() % 30 + 64);
				}
				else if (place == 1) {
					x = screenWidth + (rand() % 30 + 64);
					y = rand() % screenHeight;
				}
				else if (place == 2) {
					x = rand() % screenWidth;
					y = screenHeight + (rand() % 30 + 64);
				}
				else if (place == 3) {
					x = -(rand() % 30 + 64);
					y = rand() % screenHeight;
				}
				items.push_back(server_item(x, y, 6, itemIDNum, server));
				itemIDNum++;
		}
	}
}

void SendChat(string message, int r, int g, int b, ENetHost* server) {
	char packet[256];
	sprintf_s(packet, sizeof(packet), "Chat,%s,%i,%i,%i", message.c_str(), r, g, b);
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, p);
}

void MessagePlayer(string message, int r, int g, int b, ENetHost* server, int playerID) {
	char packet[256];
	sprintf_s(packet, sizeof(packet), "MessagePlayer,%s,%i,%i,%i,%i", message.c_str(), r, g, b, playerID);
	ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, p);
}

void LoadConfig(int& maxPlayers, int& startingHealth, int &port) {
	string file;
	string loadIP;
	vector<string> fileLines;
	fstream config("Server Config.txt");
	if (config.good()) {
		while (!config.eof()) {
			getline(config, file);
			//cout << file << endl;
			fileLines.push_back(file);
		}
		config.close();
	}
	else {
		cout << "Unable to find file..." << endl;
		cout << "Created config file. Enter information into config and restart" << endl;
		ofstream config;
		config.open("Server Config.txt");
		config << "Max Players:4\n";
		config << "Starting Health:5\n";
		config << "Port:12345";
		config.close();
		cout << "Press any key to exit..." << endl;
		system("pause>nul");
		exit(EXIT_FAILURE);
	}

	for (unsigned int i = 0; i < fileLines.size(); i++) {
		istringstream ss(fileLines[i]);
		string token;
		int filePlace = 0;

		while (getline(ss, token)) {
			int split = token.find_first_of(':');
			string key = token.substr(0, split);
			string value = token.substr(split + 1, string::npos);

			try {
				if (key == "Max Players")
					maxPlayers = stoi(value);
				else if (key == "Port")
					port = stoi(value.c_str());
				else if (key == "Starting Health")
					startingHealth = stoi(value);
			}
			catch (...) {
				cout << "Error with config" << endl;
			}
		}
	}
}
void SaveConfig(int maxPlayers, int startingHealth) {
	ofstream config;
	config.open("Server Config.txt");
	config << "Max Players:" << maxPlayers << "\n";
	config << "Starting Health:" << startingHealth;
	config.close();
}


string generatePassword() {
	string password = "";
	for (unsigned i = 0; i < 5; i++) {
		if (rand() % 2 == 0) {
			password += to_string(long(rand() % 10));
		}
		else {
			password += char(rand() % 26 + 97);
		}
	}
	return password;
}
bool IsNumber(string n) {
	return n.find_first_not_of("0123456789") == string::npos;
}
void ClearScreen()
{
	HANDLE hOut;
	COORD Position;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	Position.X = 0;
	Position.Y = 0;
	SetConsoleCursorPosition(hOut, Position);
}