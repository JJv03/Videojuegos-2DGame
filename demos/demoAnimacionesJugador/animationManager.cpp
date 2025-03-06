#include "animationManager.hpp"
#include <iostream>

AnimationManager::AnimationManager(sf::Sprite& sprite)
    : sprite(sprite), currentAnimation(nullptr), elapsedTime(0.f), currentFrame(0) {}

void AnimationManager::addAnimation(animationID id, const std::vector<Frame>& frames, bool loop) {
    animations[id] = { id, frames, loop };
}

void AnimationManager::playAnimation(animationID id) {
    std::cout << "Attempting to play animation " << id << std::endl;
    if (animations.find(id) != animations.end()) {
        currentAnimation = &animations[id];
        currentFrame = 0;
        elapsedTime = 0.f;
        std::cout << "Playing animation " << id << std::endl;
        if (!currentAnimation->frames.empty()) {
            std::cout << "Playing animation " << id << " with " << currentAnimation->frames.size() << " frames." << std::endl;
            std::cout << "Frame rect: x=" << currentAnimation->frames[0].rect.position.x
                      << ", y=" << currentAnimation->frames[0].rect.position.y
                      << ", center x =" << currentAnimation->frames[0].rect.getCenter().x
                      << ", center y =" << currentAnimation->frames[0].rect.getCenter().y << std::endl;
            try {
                if (currentAnimation->frames[0].rect.size.x > 0 && currentAnimation->frames[0].rect.size.y > 0) {
                    std::cout << "Intentando establecer rectángulo de textura..." << std::endl;
                    sprite.setTextureRect(currentAnimation->frames[0].rect);
                    std::cout << "Rectángulo de textura establecido correctamente." << std::endl;
                } else {
                    std::cerr << "Error: Invalid texture rect dimensions." << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Exception caught while setting texture rect: " << e.what() << std::endl;
            }
            std::cout << "Playing animation " << id << " with " << currentAnimation->frames.size() << " frames." << std::endl;
        } else {
            std::cerr << "Error: Animation " << id << " has no frames." << std::endl;
        }
    } else {
        std::cerr << "Error: Animation " << id << " not found." << std::endl;
    }
    std::cout << "Finished playing animation " << id << std::endl;
}

void AnimationManager::update(float deltaTime) {
    if (!currentAnimation || currentAnimation->frames.empty()) return;

    elapsedTime += deltaTime;
    if (elapsedTime >= currentAnimation->frames[currentFrame].duration) {
        elapsedTime = 0.f;
        currentFrame++;
        //std::cout << "Current frame: " << currentFrame << std::endl;

        if (currentFrame >= currentAnimation->frames.size()) {
            if (currentAnimation->loop) {
                currentFrame = 0;
            } else {
                currentFrame--;
                return; // Mantener el último frame si la animación no es en bucle
            }
        }

        sprite.setTextureRect(currentAnimation->frames[currentFrame].rect);
    }
}

bool AnimationManager::isPlaying(animationID id) {
    return currentAnimation && currentAnimation->id == id;
}

bool AnimationManager::isAnimationFinished() const {
    return currentAnimation && currentFrame == currentAnimation->frames.size() - 1;
}

void AnimationManager::resetAnimation() {
    currentFrame = 0;
    elapsedTime = 0.0f;
}

int AnimationManager::getCurrentFrameIndex() {
    return currentFrame;
}