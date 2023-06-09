#pragma once
#include "SFML/Audio.hpp"
#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H
class SoundManager {
private:
	float music125toggleTime;
	float music150toggleTime;
	sf::SoundBuffer buffer1, buffer2, buffer3, buffer4, buffer5;
	sf::Sound backgroundMusic, drinkingSound;
	sf::Time startOffset;
public:
	SoundManager();
	void initialise();
	void playDrinkingSound();
	void adjustMusic(float currentTime, int drunkLevel);
};
#endif  // SOUNDMANAGER_H