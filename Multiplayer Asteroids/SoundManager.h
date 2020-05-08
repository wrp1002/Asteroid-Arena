#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <unordered_map>
#include <filesystem>
#include <iostream>

using namespace std;

class SoundManager {
	unordered_map<string, ALLEGRO_BITMAP*> images;
	unordered_map<string, ALLEGRO_SAMPLE*> audio;

public:
	SoundManager();
	~SoundManager();

	void PlaySample(string name, float speed = -1);

	bool LoadSample(string name, string file);
	vector<string> GetNames();
};

