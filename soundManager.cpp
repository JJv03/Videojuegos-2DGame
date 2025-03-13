#include "soundManager.h"

#include <iostream>
#include <thread>

SoundManager::SoundManager() {}

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
    sounds[id].push_back(std::make_unique<sf::Sound>(soundBuffers[id]));  // Agregamos la primera instancia
}

void SoundManager::playSound(const std::string& id, float volume) {
    if (soundBuffers.find(id) == soundBuffers.end()) {
        std::cerr << "Sound " << id << " not loaded.\n";
        return;
    }

    // Buscar una instancia de sonido que no esté reproduciéndose
    for (const auto& sound : sounds[id]) {
        if (sound->getStatus() != sf::Sound::Status::Playing) {
            sound->setVolume(volume);
            sound->play();
            return;
        }
    }

    // Si todas las instancias están ocupadas, creamos una nueva
    sounds[id].push_back(std::make_unique<sf::Sound>(soundBuffers[id]));
    sounds[id].back()->setVolume(volume);
    sounds[id].back()->play();
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

    musicTracks[id].setLooping(loop);
    musicTracks[id].setVolume(volume);
    musicTracks[id].play();
}

void SoundManager::stopMusic(const std::string& id) {
    if (musicTracks.find(id) != musicTracks.end()) {
        musicTracks[id].stop();
    }
}

void SoundManager::playMusicSequence(const std::string& firstId, const std::string& secondId, bool secondSongLoop) {
    if (musicTracks.find(firstId) == musicTracks.end()) {
        std::cerr << "Music " << firstId << " not found.\n";
        return;
    }
    if (musicTracks.find(secondId) == musicTracks.end()) {
        std::cerr << "Music " << secondId << " not found.\n";
        return;
    }

    // Crear un hilo separado para gestionar la transición de las músicas
    std::thread([this, firstId, secondId, secondSongLoop]() {
        musicTracks[firstId].play();
        
        // Esperar de forma no bloqueante hasta que la primera canción termine
        while (musicTracks[firstId].getStatus() == sf::Music::Status::Playing) {
            sf::sleep(sf::milliseconds(10));
        }

        musicTracks[secondId].setLooping(secondSongLoop);
        musicTracks[secondId].play();
    }).detach(); // Desvincula el hilo para que siga ejecutándose en segundo plano
}
