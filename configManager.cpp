#include "configManager.h"
#include <fstream>
#include <iostream>

// Convierte sf::Keyboard::Key a string usando SFML
std::string configManager::to_string(sf::Keyboard::Key key) {
    return std::to_string(static_cast<int>(key)); // Guardamos como string el valor de la enum
}

// Convierte string a sf::Keyboard::Key usando SFML
sf::Keyboard::Key configManager::from_string(const std::string& keyStr) {
    int keyInt = std::stoi(keyStr); // Convertimos de string a int
    return static_cast<sf::Keyboard::Key>(keyInt); // Convertimos a sf::Keyboard::Key
}

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
    j = json{
        {"move_right", configManager::to_string(c.move_right)},
        {"move_left", configManager::to_string(c.move_left)},
        {"move_down", configManager::to_string(c.move_down)},
        {"move_up", configManager::to_string(c.move_up)},
        {"jump", configManager::to_string(c.jump)},
        {"attack", configManager::to_string(c.attack)}
    };
}

// Define how to deserialize Controls from JSON
void from_json(const json& j, configManager::Controls& c) {
    c.move_right = configManager::from_string(j.at("move_right").get<std::string>());
    c.move_left = configManager::from_string(j.at("move_left").get<std::string>());
    c.move_down = configManager::from_string(j.at("move_down").get<std::string>());
    c.move_up = configManager::from_string(j.at("move_up").get<std::string>());
    c.jump = configManager::from_string(j.at("jump").get<std::string>());
    c.attack = configManager::from_string(j.at("attack").get<std::string>());
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
    std::ifstream inputFile(file);
    if (!inputFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de configuración: " << file << std::endl;
        return;
    }

    try {
        inputFile >> currentConfig;
        inputFile.close();

        // Actualizamos solo si las claves existen y tienen el formato correcto
        if (currentConfig.contains("audio")) {
            audio = currentConfig.at("audio").get<Audio>();
        }

        if (currentConfig.contains("video")) {
            video = currentConfig.at("video").get<Video>();
        }

        if (currentConfig.contains("controls")) {
            controls = currentConfig.at("controls").get<Controls>();
        }

        if (currentConfig.contains("cheats")) {
            cheats = currentConfig.at("cheats").get<Cheats>();
        }

        if (currentConfig.contains("difficulty")) {
            difficulty = currentConfig.at("difficulty").get<Difficulty>();
        }

    } catch (const json::exception& e) {
        std::cerr << "Error al parsear el JSON: " << e.what() << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error de conversión de string a tecla (stoi): " << e.what() << std::endl;
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
