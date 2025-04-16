#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <string>

enum animationID{
    notRelevant,

    // Simon
    idleSimon,
    walkSimon,
    walkSlowSimon,
    jumpSimon,
    hurtSimon,
    deathSimon,
    attackSimon,
    duckSimon,
    attackFloorSimon,
    invulnerableSimon,
    stairAscendIdleSimon,
    stairDescendIdleSimon,
    stairAscendWalkSimon,
    stairDescendWalkSimon,
    whipUpgrade,
    //Whip
    whipLvl1StandingJumping,
    whipLvl2StandingJumping,
    whipLvl3C1StandingJumping,
    whipLvl3C2StandingJumping,
    whipLvl3C3StandingJumping,
    whipLvl3C4StandingJumping,
    whipNoAttack,
    

    
    // Enemies
    walkZombie,
    idleLeopard,
    walkLeopard,
    flyRedBat,
    sleepBat,
    flyBat,

    // Bosses
    sleepPhantomBat,
    flyPhantomBat,

    // Secondary weapon
    daggerThrowing,
    axeThrowing,
    fireBombThrowing,
    stopwatchThrowing,
    boomerangThrowing,
};


class AnimationManager {
public:
    /**
     * @struct Frame
     * @brief Represents a single frame of an animation
     *
     * Each frame consists of a texture rectangle and a duration for how long it should be displayed
     */
    struct Frame {
        sf::IntRect rect;
        float duration; // Duration of the frame in seconds
    };

    /**
     * @struct Animation
     * @brief Represents an animation sequence
     *
     * An animation consists of multiple frames and a loop flag
     */
    struct Animation {
        animationID id;
        std::vector<Frame> frames;
        bool loop;
    };

    /**
     * @brief Constructs an AnimationManager
     * @param sprite The sprite to which animations will be applied
     */
    AnimationManager(sf::Sprite& sprite);

    /**
     * @brief Adds a new animation to the manager.
     * @param name The name of the animation.
     * @param frames The frames that make up the animation.
     * @param loop Whether the animation should loop (default: true).
     */
    void addAnimation(animationID id, const std::vector<Frame>& frames, bool loop = true);
    
    /**
     * @brief Starts playing an animation.
     * @param name The name of the animation to play.
     */
    void playAnimation(animationID id);

    /**
     * @brief Starts playing an animation.
     * @param animation Struct with the frames and parameters of the animation.
     */
    void playAnimation(Animation& animation);

    /**
     * @brief Updates the animation based on elapsed time.
     * @param deltaTime Time elapsed since the last update (in seconds).
     */
    void update(float deltaTime);
    
    /**
     * @brief Checks if a specific animation is currently playing.
     * @param name The name of the animation.
     * @return True if the animation is playing, false otherwise.
     */
    bool isPlaying(animationID id);

    bool isAnimationFinished() const;

    void resetAnimation();

    int getCurrentFrameIndex();

    void setAnimationSpeed(float multiplier);

    sf::FloatRect getGlobalBounds();
    
    void setBlinking(bool enable, float interval);

    bool isBlinking() const;

private:
    sf::Sprite& sprite;
    std::unordered_map<animationID, Animation> animations;
    Animation* currentAnimation;
    float elapsedTime;
    
    std::size_t currentFrame;
    float speedMultiplier;

    bool blinking = false;
    float blinkInterval = 0.1f;
    float blinkTimer = 0.f;
    bool visible = true;
};

