#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

class configManager {
    public:
        // Singleton for global access
        static configManager& getInstance();
    
        // Load and save configuration
        void loadFromFile(const std::string& filename);
        void saveToFile(const std::string& filename);
    
        // Methods to modify configuration
        void setMasterVolume(float volume);
        void setMusicVolume(float volume);
        void setSoundVolume(float volume);
        void setWindowMode(bool fullscreen);
        void setControl(const std::string& action, sf::Keyboard::Scancode key);
        void setCheatsEnabled(bool enabled);
        void setHardModeEnabled(bool enabled);
    
        // Methods to retrieve values
        float getMasterVolume() const;
        float getMusicVolume() const;
        float getSoundVolume() const;
        bool isFullscreen() const;
        sf::Keyboard::Scancode getControl(const std::string& action) const;
        bool areCheatsEnabled() const;
        bool isHardModeEnabled() const;
    
    private:
        configManager(); // Private constructor (Singleton)
    
        // Configuration variables
        float masterVolume;
        float musicVolume;
        float soundVolume;
        bool fullscreen;
        std::map<std::string, sf::Keyboard::Scancode> controls;
        bool cheatsEnabled;
        bool hardModeEnabled;
    
        // JSON file path
        std::string configPath = "config.json";
};