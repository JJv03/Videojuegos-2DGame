#pragma once

#include <string>
#include "json.hpp"
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
        int move_right;
        int move_left;
        int move_down;
        int move_up;
        int jump;
        int attack;
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

private:
    configManager();
    Audio audio;
    Video video;
    Controls controls;
    Cheats cheats;
    Difficulty difficulty;

    json originalConfig;
    json currentConfig;
};