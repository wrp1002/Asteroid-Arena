#include "Menu.h"

Menu::Menu(string title, ALLEGRO_FONT* titleFont, ALLEGRO_FONT* buttonFont) {
	this->title = title;
	this->titleFont = titleFont;
	this->buttonFont = buttonFont;

	buttonHeight = al_get_font_line_height(buttonFont);
	buttonPadding = 20;
	enabled = false;
}

void Menu::Update(int mouseX, int mouseY) {
	for (auto &button : buttons)
		button.Update(mouseX, mouseY);
}

void Menu::Draw() {
	al_draw_text(titleFont, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 100, ALLEGRO_ALIGN_CENTER, title.c_str());

	for (auto button : buttons)
		button.Draw(buttonFont);
}

void Menu::AddButton(string name, string text, bool enabled) {
	buttons.push_back(Button(name, text, buttonFont, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + (buttonHeight + buttonPadding) * buttons.size(), enabled));
}

void Menu::UpdateButton(string name, string text){
	for (auto& button : buttons) {
		if (button.GetName() == name)
			button.SetText(text);
	}
}

void Menu::Enable() {
	enabled = true;
}

void Menu::Disable() {
	enabled = false;
}

void Menu::SetTitle(string t) {
	this->title = t;
}

string Menu::GetClicked(int mouseX, int mouseY) {
	for (auto button : buttons) {
		if (button.Clicked(mouseX, mouseY))
			return button.GetName();
	}
	return "null";
}

bool Menu::IsEnabled()
{
	return enabled;
}
