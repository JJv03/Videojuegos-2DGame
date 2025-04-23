#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <atomic>

/**
 * @brief Manages the loading and playing of sound and music assets.
 */
class SoundManager {
public:
    /**
     * @brief Constructs a SoundManager
     */
    SoundManager();

    /**
     * @brief Destructs a SoundManager
     */
    ~SoundManager();

    /**
     * @brief Loads a sound from a file and associates it with an ID.
     * @param id The unique identifier for the sound.
     * @param filepath The file path of the sound to load.
     */
    void loadSound(const std::string& id, const std::string& filepath);

    /**
     * @brief Plays a sound associated with the given ID.
     * @param id The unique identifier for the sound.
     * @param volume The volume level of the sound (default is 100).
     */
    void playSound(const std::string& id, float volume = 100.f);

    /**
     * @brief Stops all currently playing sounds.
     */
    void stopAllSounds();
    
    /**
     * @brief Stops a sound associated with the given ID.
     * @param soundToStop The unique identifier for the sound to stop.
     */
    void stopSound(const std::string& soundToStop);

    /**
     * @brief Loads a music track from a file and associates it with an ID.
     * @param id The unique identifier for the music track.
     * @param filepath The file path of the music track to load.
     */
    void loadMusic(const std::string& id, const std::string& filepath);

    /**
     * @brief Plays a music track associated with the given ID.
     * @param id The unique identifier for the music track.
     * @param loop Whether the music should loop (default is true).
     * @param volume The volume level of the music (default is 100).
     */
    void playMusic(const std::string& id, float volume = 100.f, bool loop = true);

    /**
     * @brief Stops a music track associated with the given ID.
     * @param id The unique identifier for the music track to stop.
     */
    void stopMusic(const std::string& id);

    /**
     * @brief Stops all currently playing music tracks.
     */
    void stopAllMusic();

    /**
     * @brief Plays a sequence of two music tracks. The second track plays after the first ends.
     * @param firstId The unique identifier for the first music track.
     * @param secondId The unique identifier for the second music track.
     * @param secondSongLoop Whether the second song should loop (default is true).
     */
    void playMusicSequence(const std::string& firstId, const std::string& secondId, bool secondSongLoop, float volume);

    /**
     * @brief Adjusts the volume of all active sounds.
     * @param volume The volume to set for all active sounds (0 - 100).
     */
    void adjustAllSoundVolumes(float volume);

    /**
     * @brief Adjusts the volume of all active music tracks.
     * @param volume The volume to set for all active music tracks (0 - 100).
     */
    void adjustAllMusicVolumes(float volume);

    /**
     * @brief Calculates the effective volume based on master and individual volume.
     * @param master Master volume (0 - 100).
     * @param other Specific volume (0 - 100).
     * @return The resulting combined volume (0 - 100).
     */
    float realVolume(float master, float other);

    bool musicHasFinished(const std::string& id);

private:
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    std::unordered_map<std::string, std::vector<std::unique_ptr<sf::Sound>>> sounds;
    std::unordered_map<std::string, sf::Music> musicTracks;
    std::atomic<bool> isShuttingDown;
};