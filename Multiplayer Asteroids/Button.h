#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <string>
#include <iostream>

using namespace std;

class Button {
private:
	string name, text;
	int x, y;
	int width, height;
	bool hovering, enabled;

public:
	Button(string name, string text, ALLEGRO_FONT *font, int x, int y, bool enabled = true);

	void Update(int mouseX, int mouseY);
	void Draw(ALLEGRO_FONT *font);

	bool Clicked(int mouseX, int mouseY);

	string GetName();
	void SetText(string text);
};

