#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>

static std::unordered_map<std::string, sf::Texture> textures{};
static std::unordered_map<std::string, sf::Sprite *> sprites{};
static std::unordered_map<std::string, sf::RectangleShape> rectangles{};
