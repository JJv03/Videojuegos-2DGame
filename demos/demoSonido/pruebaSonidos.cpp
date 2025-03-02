#include "soundManager.h"
#include <iostream>
#include <thread>

int main() {
    SoundManager soundManager;

    // Cargar sonidos
    soundManager.loadSound("explosion", "assets/sounds/04.wav");
    soundManager.loadSound("jump", "assets/sounds/23.wav");

    // Cargar música
    soundManager.loadMusic("background", "assets/music/05Wicked_Child(2).mp3");

    // Reproducir sonido
    std::cout << "Reproduciendo sonido de explosion..." << std::endl;
    soundManager.playSound("explosion");

    // Esperar un momento
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Reproducir otro sonido
    std::cout << "Reproduciendo sonido de salto..." << std::endl;
    soundManager.playSound("jump", 50.0f);

    // Iniciar música de fondo
    std::cout << "Reproduciendo música de fondo..." << std::endl;
    soundManager.playMusic("background", true, 75.0f);

    // Esperar unos segundos para escuchar la música
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Detener música
    std::cout << "Deteniendo música de fondo..." << std::endl;
    soundManager.stopMusic("background");

    return 0;
}
