#include "configManager.h"

configManager::configManager() {
    loadFromFile(configPath);
}

// Singleton
configManager& configManager::getInstance() {
    static configManager instance;
    return instance;
}

// Load configuration from JSON
void configManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        json j;
        file >> j;

        masterVolume = j["audio"]["master_volume"];
        musicVolume = j["audio"]["music_volume"];
        soundVolume = j["audio"]["sound_volume"];
        fullscreen = j["video"]["window_mode"] == "fullscreen";
        cheatsEnabled = j["cheats"]["enabled"];
        hardModeEnabled = j["difficulty"]["hard_mode"];

        controls["move_right"] = static_cast<sf::Keyboard::Scancode>(j["controls"]["move_right"]);
        controls["move_left"] = static_cast<sf::Keyboard::Scancode>(j["controls"]["move_left"]);
        controls["move_down"] = static_cast<sf::Keyboard::Scancode>(j["controls"]["move_down"]);
        controls["move_up"] = static_cast<sf::Keyboard::Scancode>(j["controls"]["move_up"]);
        controls["jump"] = static_cast<sf::Keyboard::Scancode>(j["controls"]["jump"]);
        controls["attack"] = static_cast<sf::Keyboard::Scancode>(j["controls"]["attack"]);
    }
}

// Save configuration to JSON
void configManager::saveToFile(const std::string& filename) {
    json j;
    j["audio"]["master_volume"] = masterVolume;
    j["audio"]["music_volume"] = musicVolume;
    j["audio"]["sound_volume"] = soundVolume;
    j["video"]["window_mode"] = fullscreen ? "fullscreen" : "windowed";
    j["cheats"]["enabled"] = cheatsEnabled;
    j["difficulty"]["hard_mode"] = hardModeEnabled;

    j["controls"]["move_right"] = static_cast<int>(controls["move_right"]);
    j["controls"]["move_left"] = static_cast<int>(controls["move_left"]);
    j["controls"]["move_down"] = static_cast<int>(controls["move_down"]);
    j["controls"]["move_up"] = static_cast<int>(controls["move_up"]);
    j["controls"]["jump"] = static_cast<int>(controls["jump"]);
    j["controls"]["attack"] = static_cast<int>(controls["attack"]);

    std::ofstream file(filename);
    file << j.dump(4);
}

// Methods to modify configuration
void configManager::setMasterVolume(float volume) { masterVolume = volume; saveToFile(configPath); }
void configManager::setMusicVolume(float volume) { musicVolume = volume; saveToFile(configPath); }
void configManager::setSoundVolume(float volume) { soundVolume = volume; saveToFile(configPath); }
void configManager::setWindowMode(bool isFullscreen) { fullscreen = isFullscreen; saveToFile(configPath); }
void configManager::setControl(const std::string& action, sf::Keyboard::Scancode key) { controls[action] = key; saveToFile(configPath); }
void configManager::setCheatsEnabled(bool enabled) { cheatsEnabled = enabled; saveToFile(configPath); }
void configManager::setHardModeEnabled(bool enabled) { hardModeEnabled = enabled; saveToFile(configPath); }

// Methods to retrieve values
float configManager::getMasterVolume() const { return masterVolume; }
float configManager::getMusicVolume() const { return musicVolume; }
float configManager::getSoundVolume() const { return soundVolume; }
bool configManager::isFullscreen() const { return fullscreen; }
sf::Keyboard::Scancode configManager::getControl(const std::string& action) const { return controls.at(action); }
bool configManager::areCheatsEnabled() const { return cheatsEnabled; }
bool configManager::isHardModeEnabled() const { return hardModeEnabled; }
