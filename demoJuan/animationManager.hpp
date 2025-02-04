#ifndef ANIMATION_MANAGER_HPP
#define ANIMATION_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <string>

class AnimationManager {
public:
    struct Frame {
        sf::IntRect rect;
        float duration; // Duración en segundos
    };

    struct Animation {
        std::vector<Frame> frames;
        bool loop;
    };

    AnimationManager(sf::Sprite& sprite);

    void addAnimation(const std::string& name, const std::vector<Frame>& frames, bool loop = true);
    void playAnimation(const std::string& name);
    void update(float deltaTime);
    void updateAnimation(bool isOnGround, bool haciaDerecha, bool haciaIzquierda, std::string& currentAnimation);
    bool isPlaying(const std::string& name);

private:
    sf::Sprite& sprite;
    std::unordered_map<std::string, Animation> animations;
    Animation* currentAnimation;
    float elapsedTime;
    std::size_t currentFrame;
};

#endif // ANIMATION_MANAGER_HPP
