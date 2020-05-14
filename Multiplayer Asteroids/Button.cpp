#include "Button.h"

Button::Button(string name, string text, ALLEGRO_FONT* font, int x, int y, bool enabled) {
	this->name = name;
	this->text = text;
	this->enabled = enabled;
	this->x = x;
	this->y = y;
	
	hovering = false;
	width = al_get_text_width(font, text.c_str());
	height = al_get_font_line_height(font);
}

void Button::Update(int mouseX, int mouseY) {
	if (enabled)
		hovering = (mouseX > x - width / 2 && mouseX < x + width / 2 && mouseY > y && mouseY < y + height);
}

void Button::Draw(ALLEGRO_FONT* font) {
	//al_draw_rectangle(x - width / 2, y, x + width / 2, y + height, al_map_rgb(255, 0, 255), 1);

	al_draw_text(font, hovering ? al_map_rgb(255, 255, 255) : al_map_rgb(155, 155, 155), x, y, ALLEGRO_ALIGN_CENTER, text.c_str());
}

bool Button::Clicked(int mouseX, int mouseY) {
	return (mouseX > x - width / 2 && mouseX < x + width / 2 && mouseY > y && mouseY < y + height);
}

bool Button::IsHovering()
{
	return hovering;
}

string Button::GetName(){
	return name;
}

void Button::SetText(string text) {
	this->text = text;
}