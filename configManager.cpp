#include "configManager.h"
#include <fstream>
#include <iostream>

// Define how to serialize Audio to JSON
void to_json(json& j, const configManager::Audio& a) {
    j = json{{"master_volume", a.master_volume},
             {"music_volume", a.music_volume},
             {"sound_volume", a.sound_volume}};
}

// Define how to deserialize Audio from JSON
void from_json(const json& j, configManager::Audio& a) {
    j.at("master_volume").get_to(a.master_volume);
    j.at("music_volume").get_to(a.music_volume);
    j.at("sound_volume").get_to(a.sound_volume);
}

// Define how to serialize Video to JSON
void to_json(json& j, const configManager::Video& v) {
    j = json{{"window_mode", v.window_mode}};
}

// Define how to deserialize Video from JSON
void from_json(const json& j, configManager::Video& v) {
    j.at("window_mode").get_to(v.window_mode);
}

// Define how to serialize Controls to JSON
void to_json(json& j, const configManager::Controls& c) {
    j = json{{"move_right", c.move_right},
             {"move_left", c.move_left},
             {"move_down", c.move_down},
             {"move_up", c.move_up},
             {"jump", c.jump},
             {"attack", c.attack}};
}

// Define how to deserialize Controls from JSON
void from_json(const json& j, configManager::Controls& c) {
    j.at("move_right").get_to(c.move_right);
    j.at("move_left").get_to(c.move_left);
    j.at("move_down").get_to(c.move_down);
    j.at("move_up").get_to(c.move_up);
    j.at("jump").get_to(c.jump);
    j.at("attack").get_to(c.attack);
}

// Define how to serialize Cheats to JSON
void to_json(json& j, const configManager::Cheats& c) {
    j = json{{"enabled", c.enabled}};
}

// Define how to deserialize Cheats from JSON
void from_json(const json& j, configManager::Cheats& c) {
    j.at("enabled").get_to(c.enabled);
}

// Define how to serialize Difficulty to JSON
void to_json(json& j, const configManager::Difficulty& d) {
    j = json{{"hard_mode", d.hard_mode}};
}

// Define how to deserialize Difficulty from JSON
void from_json(const json& j, configManager::Difficulty& d) {
    j.at("hard_mode").get_to(d.hard_mode);
}

configManager& configManager::getInstance() {
    static configManager instance;
    return instance;
}

configManager::configManager() {
    // Initialize default configuration
    // audio = {80, 70, 75};
    // video = {"fullscreen"};
    // controls = {79, 80, 81, 82, 34, 35};
    // cheats = {false};
    // difficulty = {false};

    // currentConfig["audio"] = audio;
    // currentConfig["video"] = video;
    // currentConfig["controls"] = controls;
    // currentConfig["cheats"] = cheats;
    // currentConfig["difficulty"] = difficulty;

    loadConfiguration("config.json");

    originalConfig = currentConfig; // Copy the original configuration

    std::cout<<originalConfig<<std::endl;
}

bool configManager::detectChanges() {
    return currentConfig != originalConfig;
}

void configManager::loadConfiguration(const std::string& file) {
    // Load configuration from a JSON file
    std::ifstream inputFile(file);
    if (inputFile.is_open()) {
        inputFile >> currentConfig;
        inputFile.close();

        // Update local configuration
        audio = currentConfig["audio"];
        video = currentConfig["video"];
        controls = currentConfig["controls"];
        cheats = currentConfig["cheats"];
        difficulty = currentConfig["difficulty"];
    }
}

void configManager::saveConfiguration(const std::string& file) {
    // Save configuration to a JSON file
    std::ofstream outputFile(file);
    if (outputFile.is_open()) {
        outputFile << currentConfig.dump(4); // Save with indentation
        outputFile.close();
    }
}

void configManager::applyChanges() {
    if (detectChanges()) {
        std::cout << "Applying configuration changes...\n";
        // Here you would apply changes in the game, e.g.:
        // - Adjust audio levels
        // - Change window mode (fullscreen/windowed)
        // - Update controls
        // - Apply difficulty settings

        // After applying changes, save the updated configuration
        saveConfiguration("config.json");
    }
}

// Accessors and mutators
configManager::Audio configManager::getAudio() const {
    return audio;
}

configManager::Video configManager::getVideo() const {
    return video;
}

configManager::Controls configManager::getControls() const {
    return controls;
}

configManager::Cheats configManager::getCheats() const {
    return cheats;
}

configManager::Difficulty configManager::getDifficulty() const {
    return difficulty;
}

void configManager::setAudio(const Audio& newAudio) {
    audio = newAudio;
    currentConfig["audio"] = audio;
}

void configManager::setVideo(const Video& newVideo) {
    video = newVideo;
    currentConfig["video"] = video;
}

void configManager::setControls(const Controls& newControls) {
    controls = newControls;
    currentConfig["controls"] = controls;
}

void configManager::setCheats(const Cheats& newCheats) {
    cheats = newCheats;
    currentConfig["cheats"] = cheats;
}

void configManager::setDifficulty(const Difficulty& newDifficulty) {
    difficulty = newDifficulty;
    currentConfig["difficulty"] = difficulty;
}
