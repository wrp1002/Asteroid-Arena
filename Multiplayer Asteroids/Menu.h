#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <string>
#include <vector>
#include "Button.h"
#include "client_globals.h"
#include "SoundManager.h"

using namespace std;

class Menu {
private:
	string title;
	vector<Button> buttons;
	ALLEGRO_FONT* titleFont;
	ALLEGRO_FONT* buttonFont;
	int buttonHeight, buttonPadding;
	bool enabled;


public:
	Menu(string title, ALLEGRO_FONT *titleFont, ALLEGRO_FONT *buttonFont);

	void Update(int mouseX, int mouseY, SoundManager &soundManager);
	void Draw();

	void AddButton(string name, string text, bool enabled = true);
	void UpdateButton(string name, string text);

	void Enable();
	void Disable();
	void SetTitle(string t);

	string GetClicked(int mouseX, int mouseY);
	bool IsEnabled();
};

