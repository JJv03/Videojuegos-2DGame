#include "configManager.h"
#include <fstream>
#include <iostream>

const std::unordered_map<std::string, sf::Keyboard::Scancode> configManager::stringToScancodeMap = {
    {"A", sf::Keyboard::Scancode::A}, {"B", sf::Keyboard::Scancode::B},
    {"C", sf::Keyboard::Scancode::C}, {"D", sf::Keyboard::Scancode::D},
    {"E", sf::Keyboard::Scancode::E}, {"F", sf::Keyboard::Scancode::F},
    {"G", sf::Keyboard::Scancode::G}, {"H", sf::Keyboard::Scancode::H},
    {"I", sf::Keyboard::Scancode::I}, {"J", sf::Keyboard::Scancode::J},
    {"K", sf::Keyboard::Scancode::K}, {"L", sf::Keyboard::Scancode::L},
    {"M", sf::Keyboard::Scancode::M}, {"N", sf::Keyboard::Scancode::N},
    {"O", sf::Keyboard::Scancode::O}, {"P", sf::Keyboard::Scancode::P},
    {"Q", sf::Keyboard::Scancode::Q}, {"R", sf::Keyboard::Scancode::R},
    {"S", sf::Keyboard::Scancode::S}, {"T", sf::Keyboard::Scancode::T},
    {"U", sf::Keyboard::Scancode::U}, {"V", sf::Keyboard::Scancode::V},
    {"W", sf::Keyboard::Scancode::W}, {"X", sf::Keyboard::Scancode::X},
    {"Y", sf::Keyboard::Scancode::Y}, {"Z", sf::Keyboard::Scancode::Z},
    {"Num0", sf::Keyboard::Scancode::Num0}, {"Num1", sf::Keyboard::Scancode::Num1},
    {"Num2", sf::Keyboard::Scancode::Num2}, {"Num3", sf::Keyboard::Scancode::Num3},
    {"Num4", sf::Keyboard::Scancode::Num4}, {"Num5", sf::Keyboard::Scancode::Num5},
    {"Num6", sf::Keyboard::Scancode::Num6}, {"Num7", sf::Keyboard::Scancode::Num7},
    {"Num8", sf::Keyboard::Scancode::Num8}, {"Num9", sf::Keyboard::Scancode::Num9},
    {"Space", sf::Keyboard::Scancode::Space}, {"Enter", sf::Keyboard::Scancode::Enter},
    {"LShift", sf::Keyboard::Scancode::LShift}, {"RShift", sf::Keyboard::Scancode::RShift},
    {"LControl", sf::Keyboard::Scancode::LControl}, {"RControl", sf::Keyboard::Scancode::RControl},
    {"LAlt", sf::Keyboard::Scancode::LAlt}, {"RAlt", sf::Keyboard::Scancode::RAlt},
    {"Escape", sf::Keyboard::Scancode::Escape}, {"Up", sf::Keyboard::Scancode::Up},
    {"Down", sf::Keyboard::Scancode::Down}, {"Left", sf::Keyboard::Scancode::Left},
    {"Right", sf::Keyboard::Scancode::Right}
};

const std::unordered_map<sf::Keyboard::Scancode, std::string> configManager::scancodeToStringMap = {
    {sf::Keyboard::Scancode::A, "A"}, {sf::Keyboard::Scancode::B, "B"},
    {sf::Keyboard::Scancode::C, "C"}, {sf::Keyboard::Scancode::D, "D"},
    {sf::Keyboard::Scancode::E, "E"}, {sf::Keyboard::Scancode::F, "F"},
    {sf::Keyboard::Scancode::G, "G"}, {sf::Keyboard::Scancode::H, "H"},
    {sf::Keyboard::Scancode::I, "I"}, {sf::Keyboard::Scancode::J, "J"},
    {sf::Keyboard::Scancode::K, "K"}, {sf::Keyboard::Scancode::L, "L"},
    {sf::Keyboard::Scancode::M, "M"}, {sf::Keyboard::Scancode::N, "N"},
    {sf::Keyboard::Scancode::O, "O"}, {sf::Keyboard::Scancode::P, "P"},
    {sf::Keyboard::Scancode::Q, "Q"}, {sf::Keyboard::Scancode::R, "R"},
    {sf::Keyboard::Scancode::S, "S"}, {sf::Keyboard::Scancode::T, "T"},
    {sf::Keyboard::Scancode::U, "U"}, {sf::Keyboard::Scancode::V, "V"},
    {sf::Keyboard::Scancode::W, "W"}, {sf::Keyboard::Scancode::X, "X"},
    {sf::Keyboard::Scancode::Y, "Y"}, {sf::Keyboard::Scancode::Z, "Z"},
    {sf::Keyboard::Scancode::Num0, "Num0"}, {sf::Keyboard::Scancode::Num1, "Num1"},
    {sf::Keyboard::Scancode::Num2, "Num2"}, {sf::Keyboard::Scancode::Num3, "Num3"},
    {sf::Keyboard::Scancode::Num4, "Num4"}, {sf::Keyboard::Scancode::Num5, "Num5"},
    {sf::Keyboard::Scancode::Num6, "Num6"}, {sf::Keyboard::Scancode::Num7, "Num7"},
    {sf::Keyboard::Scancode::Num8, "Num8"}, {sf::Keyboard::Scancode::Num9, "Num9"},
    {sf::Keyboard::Scancode::Space, "Space"}, {sf::Keyboard::Scancode::Enter, "Enter"},
    {sf::Keyboard::Scancode::Escape, "Escape"}, {sf::Keyboard::Scancode::Up, "Up"},
    {sf::Keyboard::Scancode::Down, "Down"}, {sf::Keyboard::Scancode::Left, "Left"},
    {sf::Keyboard::Scancode::Right, "Right"}
};

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
