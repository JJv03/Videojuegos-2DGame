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

configManager& configManager::getInstance() {
    static configManager instance;
    return instance;
}

configManager::configManager() {
    loadConfiguration("config.json");
}

void configManager::loadConfiguration(const std::string& file) {
    std::ifstream inputFile(file);
    if (!inputFile) {
        std::cerr << "Error al abrir el archivo de configuración." << std::endl;
        return;
    }
    inputFile >> originalConfig;
    currentConfig = originalConfig;

    audio.master_volume = originalConfig["audio"]["master_volume"];
    audio.music_volume = originalConfig["audio"]["music_volume"];
    audio.sound_volume = originalConfig["audio"]["sound_volume"];
    
    video.window_mode = originalConfig["video"]["window_mode"];
    
    controls.move_right = stringToScancode(originalConfig["controls"]["move_right"]);
    controls.move_left = stringToScancode(originalConfig["controls"]["move_left"]);
    controls.move_down = stringToScancode(originalConfig["controls"]["move_down"]);
    controls.move_up = stringToScancode(originalConfig["controls"]["move_up"]);
    controls.jump = stringToScancode(originalConfig["controls"]["jump"]);
    controls.attack = stringToScancode(originalConfig["controls"]["attack"]);

    cheats.enabled = originalConfig["cheats"]["enabled"];
    difficulty.hard_mode = originalConfig["difficulty"]["hard_mode"];
}

void configManager::saveConfiguration(const std::string& file) {
    std::ofstream outputFile(file);
    if (!outputFile) {
        std::cerr << "Error al guardar el archivo de configuración." << std::endl;
        return;
    }
    json j;
    j["audio"] = { {"master_volume", audio.master_volume}, {"music_volume", audio.music_volume}, {"sound_volume", audio.sound_volume} };
    j["video"] = { {"window_mode", video.window_mode} };
    j["controls"] = { {"move_right", scancodeToString(controls.move_right)},
                        {"move_left", scancodeToString(controls.move_left)},
                        {"move_down", scancodeToString(controls.move_down)},
                        {"move_up", scancodeToString(controls.move_up)},
                        {"jump", scancodeToString(controls.jump)},
                        {"attack", scancodeToString(controls.attack)} };
    j["cheats"] = { {"enabled", cheats.enabled} };
    j["difficulty"] = { {"hard_mode", difficulty.hard_mode} };

    outputFile << j.dump(4);
}

bool configManager::detectChanges() {
    return currentConfig != originalConfig;
}

void configManager::applyChanges() {
    originalConfig = currentConfig;
    saveConfiguration("config.json");
}

configManager::Audio configManager::getAudio() const { return audio; }
configManager::Video configManager::getVideo() const { return video; }
configManager::Controls configManager::getControls() const { return controls; }
configManager::Cheats configManager::getCheats() const { return cheats; }
configManager::Difficulty configManager::getDifficulty() const { return difficulty; }

void configManager::setAudio(const Audio& newAudio) { audio = newAudio; }
void configManager::setVideo(const Video& newVideo) { video = newVideo; }
void configManager::setControls(const Controls& newControls) { controls = newControls; }
void configManager::setCheats(const Cheats& newCheats) { cheats = newCheats; }
void configManager::setDifficulty(const Difficulty& newDifficulty) { difficulty = newDifficulty; }

sf::Keyboard::Scancode configManager::stringToScancode(const std::string& key) {
    auto it = stringToScancodeMap.find(key);
    return it != stringToScancodeMap.end() ? it->second : sf::Keyboard::Scancode::Unknown;
}

std::string configManager::scancodeToString(sf::Keyboard::Scancode scancode) {
    auto it = scancodeToStringMap.find(scancode);
    return it != scancodeToStringMap.end() ? it->second : "Unknown";
}