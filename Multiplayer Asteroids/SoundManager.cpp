#include "SoundManager.h"


SoundManager::SoundManager() {
	int error = 0;

	string soundsDir = "Resources/Sounds/";

	for (const auto& entry : filesystem::directory_iterator(soundsDir)) {
		string file = entry.path().string();
		file = file.substr(file.find_last_of('/') + 1, string::npos);
		error += this->LoadSample(file, soundsDir + file);
	}

	//vector<string> files = {"Alarm.wav", "Countdown.wav", "Explosion2.wav", "Explosion4.wav", "Explosion6.wav", "Explosion7.wav", "Explosion8.wav", "Explosion9.wav", "Go.wav", "Hit_Hurt3.wav", "Laser.wav", "Powerup.wav", "ShieldBreak.wav", "SmallLaser.wav", "TripleShot.wav"};


	if (error > 0) {
		printf("ERRORS LOADING!\n");
		system("pause");
	}

	printf("Loaded audio!\n");
}


SoundManager::~SoundManager() {

}



void SoundManager::PlaySample(string name, float speed) {
	if (speed == -1)
		speed = 1.0 + (rand() % 25 - 12) * .01;

	//printf("Playing '%s'\n", name.c_str());
	if (audio[name])
		al_play_sample(audio[name], 1, 0, speed, ALLEGRO_PLAYMODE_ONCE, NULL);
	else
		printf("ERROR! AUDIO NOT FOUND\n");
}


bool SoundManager::LoadSample(string name, string file) {
	printf("Loading '%s' as '%s'\n", file.c_str(), name.c_str());
	this->audio[name] = al_load_sample(file.c_str());
	return (audio[name] == NULL);
}

vector<string> SoundManager::GetNames() {
	vector<string> keys;
	for (auto item : audio) {
		keys.push_back(item.first);
	}

	return keys;
}