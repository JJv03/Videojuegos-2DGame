#include "animationManager.h"
#include <iostream>
AnimationManager::AnimationManager(sf::Sprite& sprite)
    : sprite(sprite), currentAnimation(nullptr), elapsedTime(0.f), currentFrame(0),speedMultiplier(1.0f) {}

void AnimationManager::addAnimation(animationID id, const std::vector<Frame>& frames, bool loop) {
    animations[id] = { id, frames, loop };
}

void AnimationManager::setAnimationSpeed(float multiplier) {
    speedMultiplier = multiplier;
}

void AnimationManager::playAnimation(animationID id) {
    //std::cout << "Playing animation " << id << std::endl;
    float lastHeight = sprite.getLocalBounds().size.y;

    if (animations.find(id) != animations.end()) {
        currentAnimation = &animations[id];
        currentFrame = 0;
        elapsedTime = 0.f;
        sprite.setTextureRect(currentAnimation->frames[0].rect);

        float currentHeight = sprite.getLocalBounds().size.y;
        if(lastHeight != currentHeight){
            sprite.move({0.f, lastHeight - currentHeight});
        }
    }
}

void AnimationManager::playAnimation(Animation& animation) {
    float lastHeight = sprite.getLocalBounds().size.y;

    currentAnimation = &animation;
    currentFrame = 0;
    elapsedTime = 0.f;
    sprite.setTextureRect(currentAnimation->frames[0].rect);

    float currentHeight = sprite.getLocalBounds().size.y;
    if(lastHeight != currentHeight){
        sprite.move({0.f, lastHeight - currentHeight});
    }
       
}

void AnimationManager::update(float deltaTime) {
    if (!currentAnimation || currentAnimation->frames.empty()) return;

    float lastHeight = sprite.getLocalBounds().size.y;

    elapsedTime += deltaTime*speedMultiplier;
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

        float currentHeight = sprite.getLocalBounds().size.y;
        if(lastHeight != currentHeight){
            sprite.move({0.f, lastHeight - currentHeight});
        }
           

    }
    if (blinking) {
        blinkTimer += deltaTime;
        if (blinkTimer >= blinkInterval) {
            blinkTimer = 0.f;
            visible = !visible;

            sf::Color color = sprite.getColor();
            color = visible ? sf::Color::White : sf::Color::Transparent;
            sprite.setColor(color);
        }
    }
}

bool AnimationManager::isPlaying(animationID id){
    if(!currentAnimation) return false;
    return currentAnimation->id == id;
}

bool AnimationManager::isAnimationFinished() const{
    if(!currentAnimation) return true;
    if (currentAnimation->loop) return false;   // Don't finish if it's a loop
    return currentFrame == currentAnimation->frames.size() - 1;
}

void AnimationManager::resetAnimation() {
    currentFrame = 0;
    elapsedTime = 0.0f;
}

int AnimationManager::getCurrentFrameIndex() {
    // Assuming you have a method to get the current frame index
    return currentFrame % currentAnimation->frames.size();
}

sf::FloatRect AnimationManager::getGlobalBounds(){
    if( sprite.getGlobalBounds().size.x == 0.f || 
        sprite.getGlobalBounds().size.y == 0.f){
            return sf::FloatRect();
        }
    return sprite.getGlobalBounds();
}

void AnimationManager::setBlinking(bool enable, float interval) {
    // animManager.setBlinking(true, 0.15f); Blink every 0.15 seconds
    // animManager.setBlinking(false, 0.f); Stop blinking
    blinking = enable;
    blinkInterval = interval;
    blinkTimer = 0.f;
    visible = true;
    sprite.setColor(sf::Color::White);
}

bool AnimationManager::isBlinking() const {
    return blinking;
}