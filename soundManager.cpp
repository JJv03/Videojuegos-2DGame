#include "soundManager.h"

#include <iostream>
#include <thread>

SoundManager::SoundManager() : isShuttingDown(false) {}

SoundManager::~SoundManager() {
    isShuttingDown = true;
}

void SoundManager::loadSound(const std::string& id, const std::string& filepath) {
    if (soundBuffers.find(id) != soundBuffers.end()) {
        std::cerr << "Sound " << id << " already loaded.\n";
        return;
    }

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filepath)) {
        std::cerr << "Failed to load sound: " << filepath << "\n";
        return;
    }

    soundBuffers[id] = std::move(buffer);
    sounds[id].push_back(std::make_unique<sf::Sound>(soundBuffers[id]));  // Adds the first instance
}

void SoundManager::playSound(const std::string& id, float volume) {
    if (soundBuffers.find(id) == soundBuffers.end()) {
        std::cerr << "Sound " << id << " not loaded.\n";
        return;
    }

    // Searches for a sound instance that's not playing currently
    for (const auto& sound : sounds[id]) {
        if (sound->getStatus() != sf::Sound::Status::Playing) {
            sound->setVolume(volume);
            sound->play();
            return;
        }
    }

    // If all sound instances are playing, creates a new one
    sounds[id].push_back(std::make_unique<sf::Sound>(soundBuffers[id]));
    sounds[id].back()->setVolume(volume);
    sounds[id].back()->play();
}

void SoundManager::stopSound(const std::string& soundToStop){
    for (auto& [id, soundList] : sounds) {
        if(id == soundToStop) {
            for (auto& sound : soundList) {
                if (sound) {
                    sound->stop();
                }
            }
            return; // Stop searching after stopping the sound
        }
    }
}

void SoundManager::stopAllSounds() {
    for (auto& [id, soundList] : sounds) {
        for (auto& sound : soundList) {
            if (sound) {
                sound->stop();
            }
        }
    }
}

void SoundManager::loadMusic(const std::string& id, const std::string& filepath) {
    if (musicTracks.find(id) != musicTracks.end()) {
        std::cerr << "Music " << id << " already loaded.\n";
        return;
    }

    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(filepath)) {
        std::cerr << "Failed to load music: " << filepath << "\n";
        return;
    }

    musicTracks[id] = std::move(*music);
}

void SoundManager::playMusic(const std::string& id, float volume, bool loop) {
    if (musicTracks.find(id) == musicTracks.end()) {
        std::cerr << "Music " << id << " not found.\n";
        return;
    }

    if (musicTracks[id].getStatus() == sf::SoundSource::Status::Playing) {
        // It's already playing, do nothing
        return;
    }

    musicTracks[id].setLooping(loop);
    musicTracks[id].setVolume(volume);
    musicTracks[id].play();
}

void SoundManager::stopMusic(const std::string& id) {
    if (musicTracks.find(id) != musicTracks.end()) {
        musicTracks[id].stop();
    }
}

void SoundManager::stopAllMusic() {
    for (auto& [id, music] : musicTracks) {
        music.stop();
    }
}

void SoundManager::playMusicSequence(const std::string& firstId, const std::string& secondId, bool secondSongLoop, float volume) {
    if (musicTracks.find(firstId) == musicTracks.end()) {
        std::cerr << "Music " << firstId << " not found.\n";
        return;
    }
    if (musicTracks.find(secondId) == musicTracks.end()) {
        std::cerr << "Music " << secondId << " not found.\n";
        return;
    }

    std::thread([this, firstId, secondId, secondSongLoop, volume]() {
        if (isShuttingDown) return;

        musicTracks[firstId].setVolume(volume);
        musicTracks[firstId].play();

        while (!isShuttingDown && musicTracks[firstId].getStatus() == sf::Music::Status::Playing) {
            sf::sleep(sf::milliseconds(10));
        }

        if (isShuttingDown) return;

        musicTracks[secondId].setVolume(volume);
        musicTracks[secondId].setLooping(secondSongLoop);
        musicTracks[secondId].play();
    }).detach();
}

void SoundManager::adjustAllSoundVolumes(float volume) {
    // Cycle through all active sounds and adjust their volume
    for (auto& [id, soundList] : sounds) {
        for (auto& sound : soundList) {
            if (sound->getStatus() == sf::Sound::Status::Playing) {
                sound->setVolume(volume);
            }
        }
    }
}

void SoundManager::adjustAllMusicVolumes(float volume) {
    // Cycle through all active musics and adjust their volume
    for (auto& [id, music] : musicTracks) {
        if (music.getStatus() == sf::Music::Status::Playing) {
            music.setVolume(volume);
        }
    }
}

float SoundManager::realVolume(float master, float other){
    return (master*other)/100;
}

bool SoundManager::musicHasFinished(const std::string& id){
    auto it = musicTracks.find(id);
    if (it == musicTracks.end()) {
        return true; // Considera que ha terminado si no se encuentra
    }

    return it->second.getStatus() == sf::Music::Status::Stopped;
}