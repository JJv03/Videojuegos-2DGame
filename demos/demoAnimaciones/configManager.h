#pragma once

#include <string>
#include "json.hpp"
#include <unordered_map>
#include <SFML/Window/Keyboard.hpp> // Agregado para gestionar teclas
using json = nlohmann::json;

class configManager {
public:
    // Structure to handle audio, video, controls, etc. settings
    struct Audio {
        int master_volume;
        int music_volume;
        int sound_volume;
    };

    struct Video {
        std::string window_mode;
    };

    struct Controls {
        sf::Keyboard::Scancode move_right;  
        sf::Keyboard::Scancode move_left;
        sf::Keyboard::Scancode move_down;
        sf::Keyboard::Scancode move_up;
        sf::Keyboard::Scancode jump;
        sf::Keyboard::Scancode attack;
    };

    struct Cheats {
        bool enabled;
    };

    struct Difficulty {
        bool hard_mode;
    };

    static configManager& getInstance();
    bool detectChanges();
    void loadConfiguration(const std::string& file);
    void saveConfiguration(const std::string& file);
    void applyChanges();
    
    // Accessors to get the configuration
    Audio getAudio() const;
    Video getVideo() const;
    Controls getControls() const;
    Cheats getCheats() const;
    Difficulty getDifficulty() const;

    // Methods to modify the configuration
    void setAudio(const Audio& audio);
    void setVideo(const Video& video);
    void setControls(const Controls& controls);
    void setCheats(const Cheats& cheats);
    void setDifficulty(const Difficulty& difficulty);

    // Métodos de conversión entre sf::Keyboard::Key y string
    static sf::Keyboard::Scancode stringToScancode(const std::string& key);
    static std::string scancodeToString(sf::Keyboard::Scancode scancode);

    static void to_json(json& j, const Controls& c);
    static void from_json(const json& j, Controls& c);

private:
    configManager();
    Audio audio;
    Video video;
    Controls controls;
    Cheats cheats;
    Difficulty difficulty;

    static const std::unordered_map<std::string, sf::Keyboard::Scancode> stringToScancodeMap;
    static const std::unordered_map<sf::Keyboard::Scancode, std::string> scancodeToStringMap;

    json originalConfig;
    json currentConfig;
};
