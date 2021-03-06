#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

#include <WinSock2.h>
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <enet/enet.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <ShellAPI.h>
#include <Windows.h>
#include <ctime>
#include "client_globals.h"
#include "client_player.h"
#include "client_other_player.h"
#include "client_particle.h"
#include "client_bullet.h"
#include "client_effect.h"
#include "client_asteroid.h"
#include "client_item.h"
#include "client_enemy.h"
#include "SoundManager.h"
#include "Menu.h"

using namespace std;

struct ChatMessage
{
	string text;
	int life;
	bool active;
	int alpha;
	int r, g, b;
};

void InitMenus(vector<Menu>& menus, ALLEGRO_FONT* font40, ALLEGRO_FONT* font18);
void InitStars(vector<client_particle>& particles);
void InitMainMenu(vector<Menu>& menus, vector<client_asteroid>& asteroids, ALLEGRO_BITMAP* bigAsteroidImage, ALLEGRO_BITMAP* smallAsteroidImage);

void ResetGame(client_player& player, int& gameState, ENetPeer* peer, vector<client_other_player>& players, vector<client_bullet>& bullets, vector<client_particle>& particles, vector<client_effect>& effects, vector<client_asteroid>& asteroids, vector<client_item>& items, vector<client_enemy>& enemies);
void CleanUp(vector<client_particle>& particles, vector<client_effect>& effects);

void UpdateEffects(vector<client_effect>& effects, vector<client_particle>& particles, ALLEGRO_BITMAP* explosionImage, ALLEGRO_BITMAP* flameImage, SoundManager& soundManager);

void UpdateChat(vector<ChatMessage>& chat);
void DrawChat(vector<ChatMessage>& chat, ALLEGRO_FONT* font18, client_player player);

void LoadConfig(string& ip, int& loadPort, string &username);

float GetAngle(float x1, float y1, float x2, float y2);
float GetDistance(float x1, float y1, float x2, float y2);
string GetMyIP();

int main() {
	srand(time(0));
	bool done = false;
	bool redraw = true;
	bool disconnect = false;
	bool connected = false;
	bool showInfo = false;
	bool mainMenu = true;

	int connectTimerStart = 600;
	int connectTimer = 100;
	int checkTimerStart = 1000;
	int checkTimer = checkTimerStart;

	float FPS = 60;
	int gameState = 0;
	int startTime = 400;

	string IP;
	int port = 1;
	string username;
	LoadConfig(IP, port, username);

	system("title Shooter Client");

	vector <client_other_player> players;
	vector<client_particle> particles;
	vector<client_bullet> bullets;
	vector<client_effect> effects;
	vector<client_asteroid> asteroids;
	vector<ChatMessage> chat;
	vector<client_item> items;
	vector<client_enemy> enemies;
	vector<Menu> menus;

	InitStars(particles);

	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_init_native_dialog_addon();
	al_init_image_addon();
	al_init_acodec_addon();

	if (!al_install_audio()) {
		printf("Could not install audio!\n");
		system("pause");
	}
	al_install_keyboard();
	al_install_mouse();
	enet_initialize();
	if (!al_reserve_samples(50)) {
		printf("Could not reserve samples!\n");
		system("pause");
	}

	ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);

	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
	ALLEGRO_FONT* font18 = al_load_font("Resources/Fonts/minecraft.ttf", 18, 0);
	ALLEGRO_FONT* font10 = al_load_font("Resources/Fonts/minecraft.ttf", 10, 0);
	ALLEGRO_FONT* font7 = al_load_font("Resources/Fonts/arial.ttf", 7, 0);
	ALLEGRO_FONT* font40 = al_load_font("Resources/Fonts/minecraft.ttf", 40, 0);
	ALLEGRO_FONT* titleFont = al_load_font("Resources/Fonts/minecraft.ttf", 60, 0);

	ALLEGRO_BITMAP* playerImage = al_load_bitmap("Resources/Images/ship.png");
	ALLEGRO_BITMAP* enemyImage = al_load_bitmap("Resources/Images/enemy.png");
	ALLEGRO_BITMAP* explosionImage = al_load_bitmap("Resources/Images/explosion.png");
	ALLEGRO_BITMAP* flameImage = al_load_bitmap("Resources/Images/flame.png");
	ALLEGRO_BITMAP* itemImage = al_load_bitmap("Resources/Images/Items.png");
	ALLEGRO_BITMAP* bigAsteroidImage = al_load_bitmap("Resources/Images/bigAsteroid.png");
	ALLEGRO_BITMAP* smallAsteroidImage = al_load_bitmap("Resources/Images/smallAsteroid.png");
	ALLEGRO_BITMAP* bigAsteroidsExplosion = al_load_bitmap("Resources/Images/asteroidExplosion.png");
	ALLEGRO_BITMAP* smallAsteroidsExplosion = al_load_bitmap("Resources/Images/smallAsteroidExplosion.png");
	ALLEGRO_BITMAP* shieldExplosionImage = al_load_bitmap("Resources/Images/shield explosion.png");

	ALLEGRO_BITMAP* bulletImages[5] = { al_load_bitmap("Resources/Images/bullet.png"),
										al_load_bitmap("Resources/Images/small bullet.png"),
										al_load_bitmap("Resources/Images/triple bullet.png"),
										al_load_bitmap("Resources/Images/magnet bullet.png"),
										al_load_bitmap("Resources/Images/triple bullet.png") };

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

	client_player player(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 10, username, playerImage);
	SoundManager soundManager = SoundManager();

	/*for (auto file : soundManager.GetNames()) {
		soundManager.PlaySample(file, 1);
		Sleep(100);
	}*/


	InitMenus(menus, font40, font18);
	InitMainMenu(menus, asteroids, bigAsteroidImage, smallAsteroidImage);
	menus[MAIN_MENU].UpdateButton("server", "Server: " + IP + ":" + to_string(port));
	menus[MAIN_MENU].UpdateButton("username", "Username: " + username);


	//	========================================================================================================================
	ENetAddress address;
	ENetPeer* peer = nullptr;
	ENetHost* client = nullptr;
	ENetEvent event;

	client = enet_host_create(NULL, 1, 2, 0, 0);
	if (client == NULL) {
		fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
		exit(EXIT_FAILURE);
	}
	else
		cout << "Client created" << endl;

	enet_address_set_host(&address, IP.c_str());
	address.port = port;




	al_start_timer(timer);



	while (!done) {
		while (!al_is_event_queue_empty(event_queue)) {
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);

			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				if (connected) {
					disconnect = true;
					enet_peer_disconnect(peer, 0);
				}
				else {
					done = true;
				}
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
				for (auto& menu : menus) {
					if (menu.IsEnabled()) {
						menu.Update(ev.mouse.x, ev.mouse.y, soundManager);
					}
				}
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
				if (ev.mouse.button == 1) {
					for (auto& menu : menus) {
						if (menu.IsEnabled()) {
							string name = menu.GetClicked(ev.mouse.x, ev.mouse.y);
							cout << "Clicked: " << name << endl;

							if (name != "null")
								soundManager.PlaySample("Click.wav", 1);

							if (name == "null") {}
							else if (name == "connectServer") {
								menus[MAIN_MENU].Disable();
								menus[CONNECT_MENU].Enable();
								mainMenu = false;
								connectTimer = 100;
								enet_address_set_host(&address, IP.c_str());
								address.port = port;

								menus[CONNECT_MENU].UpdateButton("connectInfo", "Server: " + IP + ":" + to_string(port));
							}
							else if (name == "connectLocalServer") {
								menus[MAIN_MENU].Disable();
								menus[CONNECT_MENU].Enable();
								mainMenu = false;
								connectTimer = 100;
								enet_address_set_host(&address, "localhost");
								address.port = port;

								menus[CONNECT_MENU].UpdateButton("connectInfo", "Server: localhost:" + to_string(port));
							}
							else if (name == "controls") {
								menus[MAIN_MENU].Disable();
								menus[CONTROLS_MENU].Enable();
							}
							else if (name == "editConfig") {
								system("cmd.exe /c \"Resources/Client Config.txt\"");
								LoadConfig(IP, port, username);
								menus[MAIN_MENU].UpdateButton("server", "Server: " + IP + ":" + to_string(port));
								menus[MAIN_MENU].UpdateButton("username", "Username: " + username);

								enet_address_set_host(&address, IP.c_str());
								address.port = port;
							}
							else if (name == "controlsBack") {
								menus[MAIN_MENU].Enable();
								menus[CONTROLS_MENU].Disable(); 
							}
							else if (name == "connectBack") {
								menus[MAIN_MENU].Enable();
								menus[CONNECT_MENU].Disable();
							}
							else if (name == "disconnectedBack") {
								menus[MAIN_MENU].Enable();
								menus[DISCONNECTED_MENU].Disable();

								InitMainMenu(menus, asteroids, bigAsteroidImage, smallAsteroidImage);
							}
							else if (name == "exit")
								done = true;

							break;
						}
					}

					for (auto& menu : menus) {
						if (menu.IsEnabled())
							menu.Update(0, 0, soundManager);
					}
				}
			}

			else if (ev.type == ALLEGRO_EVENT_DISPLAY_SWITCH_OUT) {
				player.SetKey(UP, false);
				player.SetKey(LEFT, false);
				player.SetKey(RIGHT, false);
			}
			if (ev.type == ALLEGRO_EVENT_TIMER) {
				redraw = true;
					
				for (auto& asteroid : asteroids)
					asteroid.Update();
				for (auto& enemy : enemies)
					enemy.Update(particles, effects, flameImage, soundManager);

				if (connected) {
					UpdateChat(chat);
					player.Update(particles, effects, peer, explosionImage, flameImage, soundManager);
					for (auto& particle : particles)
						particle.Update(particles);
					for (auto& bullet : bullets)
						bullet.Update(particles);
					for (auto& item : items)
						item.Update(SCREEN_WIDTH, SCREEN_HEIGHT);
					for (auto& player : players)
						player.Update(particles, effects, explosionImage, flameImage, soundManager);
					for (auto& effect : effects)
						effect.Update(effects, particles, explosionImage);

					UpdateEffects(effects, particles, explosionImage, flameImage, soundManager);

					if (disconnect) {
						char packet[256];
						int packetlen = sprintf_s(packet, sizeof(packet), "PlayerDisconnect,%i", player.GetID());
						ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, p);
					}

					CleanUp(particles, effects);
				}
			}
			if (connected) {
				if (!player.GetTyping()) {
					if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
						switch (ev.keyboard.keycode) {
						case ALLEGRO_KEY_A:
							player.SetKey(LEFT, true);
							break;

						case ALLEGRO_KEY_D:
							player.SetKey(RIGHT, true);
							break;

						case ALLEGRO_KEY_W:
							player.SetKey(UP, true);
							break;

						case ALLEGRO_KEY_M:
							player.SetKey(SHOOT, true);
							break;

						case ALLEGRO_KEY_LSHIFT:
							player.SetKey(BOOST, true);
							break;

						case ALLEGRO_KEY_TAB:
							player.SetKey(TAB, true);
							break;

						case ALLEGRO_KEY_F3:
							showInfo = !showInfo;
							break;

						case ALLEGRO_KEY_F12:
							done = true;
							break;

						case ALLEGRO_KEY_SPACE:
							if (!player.GetReady()) {
								player.SetReady(true);
								char packet[256];
								int packetlen = sprintf_s(packet, sizeof(packet), "Ready,%i", player.GetID());
								ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, p);

							}
							break;
						}
						if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER || ev.keyboard.keycode == ALLEGRO_KEY_SLASH) {
							player.SetTyping(true);
							player.SetSeen(false);
							player.SetChatMessage(player.GetName() + ": ");
							player.SetKey(UP, false);
							player.SetKey(LEFT, false);
							player.SetKey(RIGHT, false);

							if (ev.keyboard.keycode == ALLEGRO_KEY_SLASH)
								player.SetSeen(true);
						}

						player.SendPacket(peer);
					}
					else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
						if (!player.GetTyping()) {
							switch (ev.keyboard.keycode) {
							case ALLEGRO_KEY_A:
								player.SetKey(LEFT, false);
								break;

							case ALLEGRO_KEY_D:
								player.SetKey(RIGHT, false);
								break;

							case ALLEGRO_KEY_W:
								player.SetKey(UP, false);
								break;

							case ALLEGRO_KEY_M:
								player.SetKey(SHOOT, false);
								break;

							case ALLEGRO_KEY_LSHIFT:
								player.SetKey(BOOST, false);
								break;

							case ALLEGRO_KEY_TAB:
								player.SetKey(TAB, false);
								break;
							}
						}

						player.SendPacket(peer);
					}
				}
				if (player.GetTyping()) {
					if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
						ALLEGRO_USTR* input = al_ustr_new("");
						int unichar = ev.keyboard.unichar;
						if (unichar >= 32)
							al_ustr_append_chr(input, unichar);

						const char* lengthTestChar = al_cstr(input);
						string input2 = lengthTestChar;
						string name = player.GetName() + ": ";

						if (al_get_text_width(font18, player.GetChatMessage().c_str()) + al_get_text_width(font18, lengthTestChar) < SCREEN_WIDTH / 2 - 5)
							player.SetChatMessage(player.GetChatMessage() + input2);
					}
					else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
						string name = player.GetName() + ": ";
						if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
							if (player.GetChatMessage().size() > name.size()) {
								string newMessage = player.GetChatMessage();
								newMessage.erase(newMessage.size() - 1);
								player.SetChatMessage(newMessage);
							}
						}
						else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
							if (!player.GetSeen())
								player.SetSeen(true);
							else {
								player.SetTyping(false);
								if (player.GetChatMessage().size() > name.size()) {
									char packet[256];
									int packetlen = sprintf_s(packet, sizeof(packet), "Chat,%s", player.GetChatMessage().c_str());
									ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, p);
								}
								player.SetChatMessage(name);
							}
						}
						else if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
							player.SetTyping(false);
							player.SetChatMessage(name);
						}
					}
				}
			}
			else if (!connected && gameState >= 0 && !mainMenu) {
				connectTimer--;

				menus[CONNECT_MENU].UpdateButton("connectTimer", "Time until next attempt: " + to_string(connectTimer / 100));

				if (connectTimer <= 0) {
					connectTimer = connectTimerStart;

					if (peer != nullptr)
						enet_peer_reset(peer);
					peer = enet_host_connect(client, &address, 1, 0);
				}
			}
		}

		while (enet_host_service(client, &event, 0) > 0) {
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
			{
				printf("(Client) We got a new connection from %x\n", event.peer->address.host);
				//event.peer->data = "Server";
				cout << "Initializing player..." << endl;
				player = client_player(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 10, username, playerImage);
				connected = true;
				disconnect = false;
				asteroids.clear();
				menus[CONNECT_MENU].Disable();
				cout << "Waiting for level data..." << endl;
			}
			break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				int messageCount = 0;
				printf("Recieved a message from server: %s\n", event.packet->data);
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

				if (Type == "NewPlayer") {
					cout << "New Player Packet!" << endl;

					int checkID = stoi(msgVars[0].c_str());
					if (player.GetID() != checkID) {
						bool exists = false;
						for (unsigned int i = 0; i < players.size(); i++) {
							if (players[i].GetID() == checkID)
								exists = true;
						}
						if (!exists) {
							players.push_back(client_other_player(stoi(msgVars[0].c_str()), (float)stoi(msgVars[1].c_str()), (float)stoi(msgVars[2].c_str()), stof(msgVars[3].c_str()), msgVars[4], stof(msgVars[5].c_str()), playerImage, stoi(msgVars[6].c_str()), stoi(msgVars[7].c_str())));
							cout << "New Player! Size: " << players.size() << endl;
						}

						player.SendPacket(peer);
						//cout << "Player " << newPlayer.ID << ": " << newPlayer.maxHealth << "/" << newPlayer.health << endl;
					}
				}
				else if (Type == "PlayerUpdate") {
					int ID = stoi(msgVars[0].c_str());
					if (ID != player.GetID()) {
						for (unsigned int i = 0; i < players.size(); i++) {
							if (players[i].GetID() == ID) {
								players[i].PacketUpdate(stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stof(msgVars[3].c_str()), stof(msgVars[4].c_str()), stof(msgVars[5].c_str()), stoi(msgVars[6].c_str()), stoi(msgVars[7].c_str()), stoi(msgVars[8].c_str()), stoi(msgVars[9].c_str()));

								break;
							}
						}
					}
					/*else {
						if (player.GetShield() && stoi(msgVars[7].c_str()) == 0) {
							for (int i = 0; i < 1000; i++)
								particles.push_back(client_particle(player.GetX(), player.GetY(), 0, 100, 255, 1, rand() % 20, rand() % 2, rand() % 360 * (M_PI / 180), 3 + rand() % 200 * .01));
							//effects.push_back(client_effect(player.GetX() - 16, player.GetY() - 16, 0, 0, 0, 3, shieldExplosionImage));
						}

						if (stof(msgVars[4].c_str()) < player.GetHealth())
							soundManager.PlaySample("Hurt.wav");


						player.SetHealth(stof(msgVars[4].c_str()));
						player.SetShield(stoi(msgVars[7].c_str()));
					}*/
				}
				else if (Type == "PlayerID") {
					player.SetID(stoi(msgVars[0].c_str()));
					player.SetColor(stoi(msgVars[1].c_str()));
					player.SetReady(false);

					cout << "My ID:" << msgVars[0] << endl;

					char packet[256];
					int packetlen = sprintf_s(packet, sizeof(packet), "PlayerInfo,%i,%f,%f,%s", player.GetID(), player.GetX(), player.GetY(), player.GetName().c_str());
					ENetPacket* p = enet_packet_create((char*)packet, strlen(packet) + 1, ENET_PACKET_FLAG_RELIABLE);
					printf("Sent a packet to server containing %s\n", packet);
					enet_peer_send(peer, 0, p);

					player.SendPacket(peer);
				}
				else if (Type == "PlayerHealth") {
					int ID = stoi(msgVars[0].c_str());
					if (ID != player.GetID()) {
						for (unsigned int i = 0; i < players.size(); i++) {
							if (players[i].GetID() == ID) {
								players[i].SetHealth(stof(msgVars[1].c_str()));

								break;
							}
						}
					}
					else {
						float health = stof(msgVars[1].c_str());
						if (health < player.GetHealth()) {
							for (int i = 0; i < 10; i++)
								effects.push_back(client_effect(player.GetX(), player.GetY(), rand() % 361 * (M_PI / 180), rand() % 10 * .1, rand() % 10 + 10, 1, flameImage, soundManager));

							if (stof(msgVars[1].c_str()) < player.GetHealth())
								soundManager.PlaySample("Hurt.wav");
						}

						player.SetHealth(stof(msgVars[1].c_str()));
					}
				}
				else if (Type == "PlayerShield") {
					int ID = stoi(msgVars[0].c_str());
					if (ID != player.GetID()) {
						for (unsigned int i = 0; i < players.size(); i++) {
							if (players[i].GetID() == ID) {
								bool shield = stoi(msgVars[1].c_str());

								if (players[i].GetShield() && !shield) {
									for (int j = 0; j < 1000; j++)
										particles.push_back(client_particle(players[i].GetX(), players[i].GetY(), 0, 100, 255, 1, rand() % 20, rand() % 2, rand() % 360 * (M_PI / 180), 3 + rand() % 200 * .01));

									soundManager.PlaySample("ShieldBreak.wav");
								}

								players[i].SetShield(shield);

								break;
							}
						}
					}
					else {
						bool shield = stoi(msgVars[1].c_str());

						if (player.GetShield() && !shield) {
							for (int j = 0; j < 1000; j++)
								particles.push_back(client_particle(player.GetX(), player.GetY(), 0, 100, 255, 1, rand() % 20, rand() % 2, rand() % 360 * (M_PI / 180), 3 + rand() % 200 * .01));

							soundManager.PlaySample("ShieldBreak.wav");
						}

						player.SetShield(shield);
					}
				}
				else if (Type == "Weapon") {
					int ID = stoi(msgVars[0].c_str());
					if (ID == player.GetID()) {
						player.ChangeWeapon(stoi(msgVars[1].c_str()));
					}
				}
				else if (Type == "MaxHealth") {
					player.SetStartingHealth(stoi(msgVars[0].c_str()));
				}
				else if (Type == "Winner") {
					int ID = stoi(msgVars[0].c_str());
					if (ID == player.GetID()) {
						player.Win(stoi(msgVars[1].c_str()));
					}
					else {
						for (auto& otherPlayer : players) {
							if (otherPlayer.GetID() == ID)
								otherPlayer.Win(stoi(msgVars[1].c_str()));
						}
					}
				}
				else if (Type == "ErasePlayer") {
					int ID = stoi(msgVars[0].c_str());
					for (unsigned int i = 0; i < players.size(); i++) {
						if (players[i].GetID() == ID) {
							players.erase(players.begin() + i);
							cout << "Deleted Player " << ID << endl;
						}
					}
				}
				else if (Type == "SetKnockback") {
					if (stoi(msgVars[0].c_str()) == player.GetID()) {
						player.SetVelocity(stof(msgVars[1].c_str()), stof(msgVars[2].c_str()));
						player.SendPacket(peer);
					}
				}
				else if (Type == "AddKnockback") {
					if (stoi(msgVars[0].c_str()) == player.GetID()) {
						player.AddVelocity(stof(msgVars[1].c_str()), stof(msgVars[2].c_str()));
						player.SendPacket(peer);
					}
				}
				else if (Type == "Chat") {
					ChatMessage newMessage;
					newMessage.active = true;
					newMessage.life = 300;
					newMessage.alpha = 100;
					newMessage.text = msgVars[0];
					newMessage.r = stoi(msgVars[1].c_str());
					newMessage.g = stoi(msgVars[2].c_str());
					newMessage.b = stoi(msgVars[3].c_str());
					//cout << "R: " << newMessage.r << " G: " << newMessage.g << " B: " << newMessage.b << endl;
					chat.insert(chat.begin(), newMessage);
					if (chat.size() >= 15)
						chat.erase(chat.end() - 1);
				}
				else if (Type == "MessagePlayer") {
					if (player.GetID() == stoi(msgVars[4].c_str())) {
						ChatMessage newMessage;
						newMessage.active = true;
						newMessage.life = 300;
						newMessage.alpha = 100;
						newMessage.text = msgVars[0];
						newMessage.r = stoi(msgVars[1].c_str());
						newMessage.g = stoi(msgVars[2].c_str());
						newMessage.b = stoi(msgVars[3].c_str());
						chat.insert(chat.begin(), newMessage);
						if (chat.size() >= 15)
							chat.erase(chat.end() - 1);
					}
				}
				else if (Type == "NewBullet") { // players[i].GetX(), players[i].GetY(), players[i].GetDir(), 3, players[i].GetWeapon(), bulletIDNum
					float x = stof(msgVars[0].c_str());
					float y = stof(msgVars[1].c_str());
					float dir = stof(msgVars[2].c_str());
					float speed = stof(msgVars[3].c_str());
					int type = stoi(msgVars[4].c_str());
					int ID = stoi(msgVars[5].c_str());
					bullets.push_back(client_bullet(x, y, dir, type, speed, ID, bulletImages[type]));
						
					switch(type) {
						case 0:
							soundManager.PlaySample("Laser.wav");
							break;
						case 1:
							soundManager.PlaySample("SmallLaser.wav");
							break;
						case 2:
							soundManager.PlaySample("TripleShot.wav");
							break;
						case 3:
							soundManager.PlaySample("Magnet.wav");
							break;
						case 4:
							soundManager.PlaySample("TripleShot.wav");
							break;
					}
				}
				else if (Type == "BulletUpdate") {
					int ID = stoi(msgVars[0].c_str());
					float x = stof(msgVars[1].c_str());
					float y = stof(msgVars[2].c_str());
					float velX = stof(msgVars[3].c_str());
					float velY = stof(msgVars[4].c_str());
					for (unsigned i = 0; i < bullets.size(); i++) {
						if (bullets[i].GetID() == ID) {
							bullets[i].PacketUpdate(x, y, velX, velY);
						}
					}
				}
				else if (Type == "EraseBullet") {
					int ID = stoi(msgVars[0].c_str());
					for (unsigned i = 0; i < bullets.size(); i++) {
						if (bullets[i].GetID() == ID) {
							if (bullets[i].GetType() == 0) {
								int amount = rand() % 10 + 15;
								for (int j = 0; j < amount; j++) {
									particles.push_back(client_particle(bullets[i].GetX() + bullets[i].GetImageSize() / 2, bullets[i].GetY() + bullets[i].GetImageSize() / 2, rand() % 255, rand() % 255, rand() % 255, 1, rand() % 10 + 10, 0, (rand() % 361 - 180) * (M_PI / 180), rand() % 200 * .01));
								}
							}
							else if (bullets[i].GetType() == 1) {
								int amount = rand() % 3 + 2;
								for (int j = 0; j < amount; j++) {
									particles.push_back(client_particle(bullets[i].GetX() + bullets[i].GetImageSize() / 2, bullets[i].GetY() + bullets[i].GetImageSize() / 2, 255, 255, 255, 1, rand() % 10 + 10, 0, (rand() % 361 - 180) * (M_PI / 180), rand() % 200 * .01));
								}
							}
							else if (bullets[i].GetType() == 2 || bullets[i].GetType() == 4) {
								int amount = rand() % 5 + 10;
								for (int j = 0; j < amount; j++) {
									particles.push_back(client_particle(bullets[i].GetX() + bullets[i].GetImageSize() / 2, bullets[i].GetY() + bullets[i].GetImageSize() / 2, 255, rand() % 255, 0, 1, rand() % 10 + 10, 0, (rand() % 361 - 180) * (M_PI / 180), rand() % 200 * .01));
								}
							}

							//if (bullets[i].GetType() != 3)
							//	soundManager.PlaySample("BulletBreak.wav");

							bullets.erase(bullets.begin() + i);
							break;
						}
					}
				}
				else if (Type == "EraseAsteroid") {
					int ID = stoi(msgVars[0].c_str());
					for (unsigned i = 0; i < asteroids.size(); i++) {
						if (asteroids[i].GetID() == ID) {
							if (asteroids[i].GetSize() == 0) {
								effects.push_back(client_effect(asteroids[i].GetX() - 16, asteroids[i].GetY() - 16, 0, 0, 0, 2, smallAsteroidsExplosion, soundManager));
							}
							else {
								effects.push_back(client_effect(asteroids[i].GetX() - 32, asteroids[i].GetY() - 32, 0, 0, 0, 2, bigAsteroidsExplosion, soundManager));
							}

							for (int j = 0; j < 100 + asteroids[i].GetSize() * 100; j++) {
								int color = 100 + (rand() % 31 - 15);
								float dir = rand() % 361 * (M_PI / 180);
								int amountX = rand() % (16 + asteroids[i].GetSize() * 16) * cos(dir);
								int amountY = rand() % (16 + asteroids[i].GetSize() * 16) * sin(dir);
								particles.push_back(client_particle(asteroids[i].GetX() + amountX, asteroids[i].GetY() + amountY, color, color, color, 3, 50 + rand() % 100, rand() % 200 * .01, dir, rand() % 30 * .01 + .1));
							}

							asteroids.erase(asteroids.begin() + i);
							soundManager.PlaySample("AsteroidBreak.wav");
							break;
						}
					}
				}
				else if (Type == "NewAsteroid") {
					cout << "New asteroid!" << endl;
					asteroids.push_back(client_asteroid(stoi(msgVars[0].c_str()), stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stof(msgVars[3].c_str()), stof(msgVars[4].c_str()), stoi(msgVars[5].c_str()), bigAsteroidImage, smallAsteroidImage));
				}
				else if (Type == "AsteroidUpdate") {
					int ID = stoi(msgVars[0].c_str());
					for (unsigned i = 0; i < asteroids.size(); i++) {
						if (asteroids[i].GetID() == ID) {
							asteroids[i].PacketUpdate(stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stof(msgVars[3].c_str()), stof(msgVars[4].c_str()));
						}
					}
				}
				else if (Type == "NewItem") {
					items.push_back(client_item(stoi(msgVars[0].c_str()), stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stoi(msgVars[3].c_str()), stof(msgVars[4].c_str()), stof(msgVars[5].c_str())));
				}
				else if (Type == "ItemUpdate") {
					int ID = stoi(msgVars[0].c_str());
					for (unsigned i = 0; i < items.size(); i++) {
						if (items[i].GetID() == ID) {
							items[i].PacketUpdate(stof(msgVars[1].c_str()), stof(msgVars[2].c_str()), stof(msgVars[3].c_str()));
							break;
						}
					}
				}
				else if (Type == "EraseItem") {
					int ID = stoi(msgVars[0].c_str());
					for (unsigned i = 0; i < items.size(); i++) {
						if (items[i].GetID() == ID) {
							items.erase(items.begin() + i);
							break;
						}
					}
				}
				else if (Type == "ItemSound") {
					soundManager.PlaySample("Powerup.wav");
				}
				else if (Type == "StartingHealth") {
					player.SetNewHealth(stoi(msgVars[0].c_str()));
					if (gameState == 0)
						player.SetStartingHealth(stoi(msgVars[0].c_str()));
				}
				else if (Type == "StartTimer") {
					startTime = stoi(msgVars[0].c_str());
					if (gameState == 0) {
						if (startTime > 50)
							soundManager.PlaySample("Countdown.wav", 1);
						else {
							soundManager.PlaySample("Go.wav", 1);
							gameState = 1;
						}
					}
				}
				else if (Type == "Kick") {
					//kickMessage = msgVars[0];
					menus[DISCONNECTED_MENU].SetTitle(msgVars[0]);
					menus[DISCONNECTED_MENU].Enable();

					enet_peer_disconnect(peer, 0);
				}
				else if (Type == "ResetGame") {
					ResetGame(player, gameState, peer, players, bullets, particles, effects, asteroids, items, enemies);
					player.SendPacket(peer);
					cout << "Reset Game" << endl;
				}
				else if (Type == "NewEnemy") {
					int ID = stoi(msgVars[0].c_str());
					float x = stof(msgVars[1].c_str());
					float y = stof(msgVars[2].c_str());
					float dir = stof(msgVars[3].c_str());
					float shootDir = stof(msgVars[4].c_str());
					float maxHealth = stof(msgVars[5].c_str());

					enemies.push_back(client_enemy(ID, x, y, dir, shootDir, maxHealth, enemyImage));

					soundManager.PlaySample("Alarm.wav", 1);
				}
				else if (Type == "EnemyUpdate") {
					int ID = stoi(msgVars[0].c_str());
					float x = stof(msgVars[1].c_str());
					float y = stof(msgVars[2].c_str());
					float dir = stof(msgVars[3].c_str());
					float shootDir = stof(msgVars[4].c_str());
					float health = stof(msgVars[5].c_str());

					for (unsigned i = 0; i < enemies.size(); i++) {
						if (enemies[i].GetID() == ID) {
							enemies[i].PacketUpdate(x, y, dir, shootDir, health);
						}
					}
				}
				else if (Type == "EnemyHurt") {
					soundManager.PlaySample("Hurt.wav");
				}
				else if (Type == "EraseEnemy") {
					int ID = stoi(msgVars[0].c_str());
					for (unsigned i = 0; i < enemies.size(); i++) {
						if (enemies[i].GetID() == ID) {
							
							for (int j = 0; j < 10; j++)
								effects.push_back(client_effect(enemies[i].GetX() - 32 + rand() % 33 - 16, enemies[i].GetY() - 32 + rand() % 33 - 16, 0, 0, rand() % 5 - 3, 0, explosionImage, soundManager));
							for (int j = 0; j < 20; j++)
								effects.push_back(client_effect(enemies[i].GetX(), enemies[i].GetY(), (rand() % 361 - 180) * (M_PI / 180), 3 + rand() % 3, rand() % 20 + 10, 1, flameImage, soundManager));
							for (int j = 0; j < 5000; j++)
								particles.push_back(client_particle(enemies[i].GetX(), enemies[i].GetY(), 255, rand() % 255, 0, 1, rand() % 30, rand() % 2, rand() % 360 * (M_PI / 180), 3 + rand() % 200 * .01));
							
							enemies.erase(enemies.begin() + i);
							soundManager.PlaySample("EnemyDeath.wav", 1);

							break;
						}
					}
				}

				enet_packet_destroy(event.packet);
			}

			break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("(Client) %s disconnected.\n", event.peer->data);
				players.clear();
				asteroids.clear();
				items.clear();
				effects.clear();
				connected = false;
				mainMenu = true;
				gameState = 0;

				if (!menus[DISCONNECTED_MENU].IsEnabled()) {
					menus[DISCONNECTED_MENU].SetTitle("Disconnected");
					menus[DISCONNECTED_MENU].Enable();
				}
			

				break;
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(0, 0, 0));

			for (auto particle : particles)
				particle.Draw();
			for (auto asteroid : asteroids)
				asteroid.Draw(showInfo);

			if (!mainMenu) {
				if (gameState >= 0) {
					if (connected) {
							
						for (auto item : items)
							item.Draw(itemImage, showInfo);
						for (auto bullet : bullets)
							bullet.Draw(display, showInfo);
						for (auto player : players)
							player.Draw(display, font7, font18, showInfo);
						for (auto enemy : enemies)
							enemy.Draw();

						player.Draw(display, font18, showInfo);

						for (auto effect : effects)
							effect.Draw(showInfo);


						al_draw_filled_rectangle(0, 0, 100, 5, al_map_rgb(255, 0, 0));
						al_draw_filled_rectangle(0, 0, 100 * (player.GetHealth() / player.GetMaxHealth()), 5, al_map_rgb(0, 255, 0));

						al_draw_filled_rectangle(SCREEN_WIDTH, 0, SCREEN_WIDTH - 100 * (1), 5, al_map_rgb(255, 0, 170));
						al_draw_filled_rectangle(SCREEN_WIDTH, 0, SCREEN_WIDTH - 100 * (player.GetChargeNum() / player.GetChargeNumMax()), 5, al_map_rgb(0, 0, 255));

						if (player.GetReady() && startTime < 204) {
							if (gameState == 0) {
								al_draw_textf(font40, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20, ALLEGRO_ALIGN_CENTER, "%i", (startTime - 50) / 50);
							}
							else {
								al_draw_text(font40, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20, ALLEGRO_ALIGN_CENTER, "GO");
							}
						}
						if (player.GetTyping()) {
							al_draw_filled_rectangle(5, SCREEN_HEIGHT - 30, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 10, al_map_rgba(50, 50, 50, 100));
							al_draw_textf(font18, al_map_rgb(255, 255, 255), 7, SCREEN_HEIGHT - 30, NULL, "%s", player.GetChatMessage().c_str());
						}
						DrawChat(chat, font18, player);

						if (player.GetKey(TAB)) {
							vector<int> lengths;
							vector<string> names;
							names.push_back("Wins");
							names.push_back(player.GetName() + ": " + to_string(player.GetWins()));

							for (auto otherPlayer : players)
								names.push_back(otherPlayer.GetName() + ": " + to_string(otherPlayer.GetWins()));

							for (auto name : names) {
								lengths.push_back(al_get_text_width(font10, name.c_str()) / 2 + 10);
							}

							int maxLength = 0;
							for (auto length : lengths) {
								if (length > maxLength)
									maxLength = length;
							}

							for (unsigned i = 0; i < names.size(); i++) {
								al_draw_filled_rectangle(SCREEN_WIDTH / 2 - maxLength, i * 14, SCREEN_WIDTH / 2 + maxLength, i * 14 + 14, al_map_rgb(75, 75, 75));
								al_draw_rectangle(SCREEN_WIDTH / 2 - maxLength, i * 14, SCREEN_WIDTH / 2 + maxLength, i * 14 + 14, al_map_rgb(25, 25, 25), 2);
								al_draw_text(font10, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, i * 14 + 2, ALLEGRO_ALIGN_CENTER, names[i].c_str());
							}
						}

						if (!player.GetReady()) {
							al_draw_text(font18, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "Press Space To Start");
						}
					}
				}

				/*if (!connected && gameState >= 0) {
					al_draw_textf(font18, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50, ALLEGRO_ALIGN_CENTRE, "Attempting to connect to server %s:%i", IP.c_str(), port);
					al_draw_textf(font18, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 30, ALLEGRO_ALIGN_CENTRE, "Time until next attempt: %i", connectTimer / 100);
					al_draw_text(font18, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Change IP in config.txt");
				}*/
			}

			for (auto menu : menus) {
				if (menu.IsEnabled())
					menu.Draw();
			}

			al_flip_display();
		}
	}

	al_destroy_font(font18);
	al_destroy_font(font10);
	al_destroy_font(font40);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);

	enet_host_destroy(client);
}

void InitMenus(vector<Menu>& menus, ALLEGRO_FONT *font40, ALLEGRO_FONT *font18) {
	Menu main = Menu("Asteroid Arena", font40, font18);

	main.AddButton("connectServer", "Connect to Server");
	main.AddButton("connectLocalServer", "Connect to Local Server");
	main.AddButton("controls", "Controls");	
	main.AddButton("editConfig", "Edit Config");
	main.AddButton("exit", "Exit");

	main.AddButton("space", "");
	main.AddButton("server", "Server: 123.123.123.123:12232", false);
	main.AddButton("username", "Username: username", false);
	main.Enable();


	Menu controlsMenu("Controls", font40, font18);
	controlsMenu.AddButton("movement", "WASD to move", false);
	controlsMenu.AddButton("boost", "Shift to activate boost", false);
	controlsMenu.AddButton("shoot", "M to fire weapon", false);
	controlsMenu.AddButton("controlsBack", "Back");


	Menu connectionMenu("Attempting to connect to server...", font40, font18);
	connectionMenu.AddButton("connectInfo", "IP", false);
	connectionMenu.AddButton("connectTimer", "Time until next attempt: ", false);
	connectionMenu.AddButton("connectBack", "Back");


	Menu disconnectMenu("Disconnected", font40, font18);
	disconnectMenu.AddButton("disconnectedBack", "Return to Main Menu");


	menus.push_back(main);
	menus.push_back(controlsMenu);
	menus.push_back(connectionMenu);
	menus.push_back(disconnectMenu);
}

void ResetGame(client_player& player, int& gameState, ENetPeer* peer, vector<client_other_player>& players, vector<client_bullet>& bullets, vector<client_particle>& particles, vector<client_effect>& effects, vector<client_asteroid>& asteroids, vector<client_item>& items, vector<client_enemy> &enemies) {
	gameState = 0;

	bullets.clear();
	particles.clear();
	effects.clear();
	player.Reset(SCREEN_WIDTH, SCREEN_HEIGHT);
	asteroids.clear();
	items.clear();
	enemies.clear();

	for (auto& otherPlayer : players)
		otherPlayer.Reset();

	InitStars(particles);
	player.SendPacket(peer);
}

void CleanUp(vector<client_particle>& particles, vector<client_effect>& effects) {
	for (unsigned i = 0; i < particles.size();) {
		if (!particles[i].GetActive())
			particles.erase(particles.begin() + i);
		else
			i++;
	}

	for (unsigned i = 0; i < effects.size();) {
		if (!effects[i].GetActive())
			effects.erase(effects.begin() + i);
		else
			i++;
	}
}

void InitStars(vector<client_particle>& particles) {
	for (int i = 0; i < rand() % 10 + 20; i++) {
		int color = rand() % 100 + 100;
		particles.push_back(client_particle(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, color, color, color, 2, 0, 1, 0, 0));
	}
}

void InitMainMenu(vector<Menu>& menus, vector<client_asteroid>& asteroids, ALLEGRO_BITMAP *bigAsteroidImage, ALLEGRO_BITMAP *smallAsteroidImage) {
	for (int i = 0; i < 10; i++)
		asteroids.push_back(client_asteroid(-1, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, (rand() % 360) * (M_PI / 180), 1, rand() % 2, bigAsteroidImage, smallAsteroidImage));

	for (int i = 0; i < 100; i++) {
		for (auto& a : asteroids)
			a.Update();
	}

	for (auto& menu : menus)
		menu.Disable();

	menus[MAIN_MENU].Enable();
}

void UpdateEffects(vector<client_effect>& effects, vector<client_particle>& particles, ALLEGRO_BITMAP* explosionImage, ALLEGRO_BITMAP* flameImage, SoundManager &soundManager) {
	for (unsigned i = 0; i < effects.size(); i++) {
		if (effects[i].GetType() == 1) {
			if (rand() % 5 == 0)
				particles.push_back(client_particle(effects[i].GetX() - 6 * cos(effects[i].GetDir() * (M_PI / 180)), effects[i].GetY() - 6 * sin(effects[i].GetDir() * (M_PI / 180)), 255, rand() % 255, 0, 0, 30, rand() % 3, effects[i].GetDir() * (M_PI / 180) + (rand() % 61 - 30) * (M_PI / 180) + M_PI, rand() % 100 * .01));
			if (effects[i].GetSpeed() >= 3 && effects[i].GetSpeed() <= 5) {
				if (rand() % 2 == 0)
					effects.push_back(client_effect(effects[i].GetX() - 32, effects[i].GetY() - 32, 0, 0, rand() % 5 - 3, 0, explosionImage, soundManager));
				if (rand() % 3 == 0)
					effects.push_back(client_effect(effects[i].GetX(), effects[i].GetY(), (rand() % 361 - 180) * (M_PI / 180), 2 + rand() % 2, rand() % 20 - 10, 1, flameImage, soundManager));
				if (rand() % 2 == 0)
					effects.push_back(client_effect(effects[i].GetX(), effects[i].GetY(), (rand() % 361 - 180) * (M_PI / 180), 2, rand() % 20 + 20, 1, flameImage, soundManager));
			}
		}
	}
}

void UpdateChat(vector<ChatMessage>& chat) {
	for (unsigned int i = 0; i < chat.size(); i++) {
		if (chat[i].life > 0)
			chat[i].life--;
		if (chat[i].life < 100 && chat[i].alpha > 0)
			chat[i].alpha--;
	}
}
void DrawChat(vector<ChatMessage>& chat, ALLEGRO_FONT* font18, client_player player) {
	for (unsigned int i = 0; i < chat.size(); i++) {
		if (chat[i].life > 0 || player.GetTyping()) {
			if (player.GetTyping()) {
				al_draw_filled_rectangle(5, SCREEN_HEIGHT - (20 * (i + 1)) - 30, SCREEN_WIDTH / 2, SCREEN_HEIGHT - (20 * (i + 1)) - 10, al_map_rgba(50, 50, 50, 100));
				al_draw_textf(font18, al_map_rgb(chat[i].r, chat[i].g, chat[i].b), 7, SCREEN_HEIGHT - (20 * (i + 1)) - 28, NULL, "%s", chat[i].text.c_str());
			}
			else if (chat[i].life > 0) {
				al_draw_filled_rectangle(5, SCREEN_HEIGHT - (20 * (i + 1)) - 30, SCREEN_WIDTH / 2, SCREEN_HEIGHT - (20 * (i + 1)) - 10, al_map_rgba(50, 50, 50, chat[i].alpha));
				al_draw_textf(font18, al_map_rgba(chat[i].r, chat[i].g, chat[i].b, chat[i].alpha), 7, SCREEN_HEIGHT - (20 * (i + 1)) - 28, NULL, "%s", chat[i].text.c_str());
			}
		}
	}
}

void LoadConfig(string &ip, int& loadPort, string &username) {
	string file;
	vector<string> fileLines;
	fstream config("Resources/Client Config.txt");
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
		cout << "Created config file. Reloading file now..." << endl;
		ofstream config;
		config.open("Resources/Client Config.txt");
		config << "IP:123.123.123.123\n";
		config << "Port:12345\n";
		config << "Username:User" + to_string(100 + rand() % 800) + "\n";
		config << "Show Console:0";
		config.close();
		/*cout << "Press any key to exit..." << endl;
		system("pause>nul");
		exit(EXIT_FAILURE);*/

		LoadConfig(ip, loadPort, username);
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
				if (key == "IP")
					ip = value;
				else if (key == "Port")
					loadPort = stoi(value.c_str());
				else if (key == "Username")
					username = value;
				else if (key == "Show Console") {
					if (stoi(value))
						ShowWindow(GetConsoleWindow(), SW_SHOW);
					else
						ShowWindow(GetConsoleWindow(), SW_HIDE);
				}
			}
			catch (...) {
				cout << "Error with config" << endl;
			}
		}
	}

	cout << "Config loaded   " << "IP:'" << ip << "' Port:'" << loadPort << "' Username:'" << username << "'" << endl;
}

float GetAngle(float x1, float y1, float x2, float y2) {
	return atan2(y1 - y2, x1 - x2);
}
float GetDistance(float x1, float y1, float x2, float y2) {
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}
string GetMyIP() {
	char szBuffer[1024];

#ifdef WIN32
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 0);
	if (::WSAStartup(wVersionRequested, &wsaData) != 0)
		return "localhost";
#endif


	if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return "localhost";
	}

	struct hostent* host = gethostbyname(szBuffer);
	if (host == NULL)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return "localhost";
	}

	//Obtain the computer's IP
	unsigned char b1, b2, b3, b4;
	b1 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b1;
	b2 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b2;
	b3 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b3;
	b4 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b4;

#ifdef WIN32
	WSACleanup();
#endif
	return to_string((long)b1) + "." + to_string((long)b2) + "." + to_string((long)b3) + "." + to_string((long)b4);
}