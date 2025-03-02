#include "animationManager.hpp"
#include <iostream>
AnimationManager::AnimationManager(sf::Sprite& sprite)
    : sprite(sprite), currentAnimation(nullptr), elapsedTime(0.f), currentFrame(0) {}

void AnimationManager::addAnimation(animationID id, const std::vector<Frame>& frames, bool loop) {
    animations[id] = { id, frames, loop };
}

void AnimationManager::playAnimation(animationID id) {
    //std::cout << "Playing animation " << id << std::endl;
    if (animations.find(id) != animations.end()) {
        currentAnimation = &animations[id];
        currentFrame = 0;
        elapsedTime = 0.f;
        sprite.setTextureRect(currentAnimation->frames[0].rect);
    }
}


void AnimationManager::update( float deltaTime) {
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

bool AnimationManager::isPlaying(animationID id){
    return currentAnimation->id == id;
}

bool AnimationManager::isAnimationFinished() const{
    return currentFrame == currentAnimation->frames.size() - 1;
}

void AnimationManager::resetAnimation() {
    currentFrame = 0;
    elapsedTime = 0.0f;
}

int AnimationManager::getCurrentFrameIndex() {
    // Assuming you have a method to get the current frame index
    return currentFrame;
}