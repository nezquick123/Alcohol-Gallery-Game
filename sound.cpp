#include "sound.h"
#include "SFML/Audio.hpp"
#include <iostream>
using namespace std;
	SoundManager::SoundManager() {
		
	}

	void SoundManager::initialise() {
		if (!buffer1.loadFromFile("entertainer.wav")
			|| !buffer2.loadFromFile("drink.wav")
			|| !buffer3.loadFromFile("entertainer125.wav")
			|| !buffer4.loadFromFile("entertainer150.wav")
			|| !buffer5.loadFromFile("entertainer175.wav")) {
			cout << "Cos sie popsulo";
		}

		backgroundMusic.setBuffer(buffer1);
		drinkingSound.setBuffer(buffer2);

		backgroundMusic.setVolume(20);
		backgroundMusic.play();
	}

	void SoundManager::playDrinkingSound() {
		drinkingSound.play();
	}

	void SoundManager::adjustMusic(float currentTime, int drunkLevel) {
		if (drunkLevel == 3) {
			backgroundMusic.setBuffer(buffer3);
			startOffset = sf::seconds(currentTime * 100.0 / 125.0);
			music125toggleTime = currentTime;
			backgroundMusic.setPlayingOffset(startOffset);
			backgroundMusic.play();
		}
		if (drunkLevel == 6) {
			backgroundMusic.setBuffer(buffer4);
			float normalSongTime = music125toggleTime + (currentTime - music125toggleTime) * 125 / 100;
			startOffset = sf::seconds(normalSongTime * 100.0 / 150.0);
			music150toggleTime = currentTime;
			backgroundMusic.setPlayingOffset(startOffset);
			backgroundMusic.play();
		}
		if (drunkLevel == 9) {
			backgroundMusic.setBuffer(buffer5);
			float normalSongTime = music125toggleTime + (music150toggleTime - music125toggleTime) * 125 / 100 + (currentTime - music150toggleTime) * 150 / 100;
			startOffset = sf::seconds(normalSongTime * 100.0 / 175.0);
			backgroundMusic.setPlayingOffset(startOffset);
			backgroundMusic.play();
		}
	}