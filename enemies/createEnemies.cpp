#include "createEnemies.h"

// Creates a zombie enemy with proper sprite and hitbox setup
Zombie *createZombie(const sf::Vector2f &position)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect ZOMBIE_SPRITE_REGION = {{1, 28}, {16, 32}};
    const float HITBOX_WIDTH = 12.f;
    const float HITBOX_HEIGHT = 30.f;

    // Configure sprite
    auto zombieSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    zombieSprite->setTextureRect(ZOMBIE_SPRITE_REGION);
    zombieSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = zombieSprite->getLocalBounds();
    zombieSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Zombie(zombieSprite, hitboxes);
}

// Creates a leopard enemy with level-specific settings
Leopard *createLeopard(const sf::Vector2f &position, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect LEOPARD_SPRITE_REGION = {{56, 11}, {24, 16}};
    const float HITBOX_WIDTH = 22.f;
    const float HITBOX_HEIGHT = 18.f;

    // Configure sprite
    auto leopardSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    leopardSprite->setTextureRect(LEOPARD_SPRITE_REGION);
    leopardSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = leopardSprite->getLocalBounds();
    leopardSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Leopard(leopardSprite, hitboxes, level, stage);
}

// Creates a bat enemy with spawn zone parameters
Bat *createBatSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect BAT_SPRITE_REGION = {{184, 11}, {16, 16}};
    const float HITBOX_WIDTH = 15.f;
    const float HITBOX_HEIGHT = 15.f;

    // Configure sprite
    auto batSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    batSprite->setTextureRect(BAT_SPRITE_REGION);
    batSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = batSprite->getLocalBounds();
    batSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Bat(batSprite, hitboxes, position, zoneSize, level, stage);
}

// Creates a fishman enemy with spawn zone parameters
FishMan *createFishManSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage, std::mt19937 &rngReference)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect FISHMAN_SPRITE_REGION = {{309, 28}, {16, 32}};
    const float HITBOX_WIDTH = 15.f;
    const float HITBOX_HEIGHT = 28.f;

    // Configure sprite
    auto fishmanSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    fishmanSprite->setTextureRect(FISHMAN_SPRITE_REGION);
    fishmanSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = fishmanSprite->getLocalBounds();
    fishmanSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new FishMan(fishmanSprite, hitboxes, position, zoneSize, level, stage, rngReference);
}

// Creates a ghost enemy with level-specific settings
Ghost *createGhost(const sf::Vector2f &position, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect LEOPARD_SPRITE_REGION = {{1, 75}, {16, 16}};
    const float HITBOX_WIDTH = 15.f;
    const float HITBOX_HEIGHT = 15.f;

    // Configure sprite
    auto ghostSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    ghostSprite->setTextureRect(LEOPARD_SPRITE_REGION);
    ghostSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = ghostSprite->getLocalBounds();
    ghostSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Ghost(ghostSprite, hitboxes, level, stage);
}

// Creates a medusa enemy with spawn zone parameters
Medusa *createMedusaSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect BAT_SPRITE_REGION = {{119, 75}, {16, 16}};
    const float HITBOX_WIDTH = 15.f;
    const float HITBOX_HEIGHT = 15.f;

    // Configure sprite
    auto medusaSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    medusaSprite->setTextureRect(BAT_SPRITE_REGION);
    medusaSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = medusaSprite->getLocalBounds();
    medusaSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Medusa(medusaSprite, hitboxes, position, zoneSize, level, stage);
}


Cannon *createCannon(const sf::Vector2f &position, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect CANNON_SPRITE_REGION = {{40, 75}, {16, 32}};
    const float HITBOX_WIDTH = 16.f;
    const float HITBOX_HEIGHT = 32.f;

    // Configure sprite
    auto cannonSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    cannonSprite->setTextureRect(CANNON_SPRITE_REGION);
    cannonSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = cannonSprite->getLocalBounds();
    cannonSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };
    return new Cannon(cannonSprite, hitboxes, level, stage);
}

AxeMan *createAxeMan(const sf::Vector2f &position, const size_t &level, const size_t &stage){
    // Sprite sheet coordinates and dimensions
    const sf::IntRect AXEMAN_SPRITE_REGION = {{240, 122}, {24, 32}};
    const float HITBOX_WIDTH = 22.f;
    const float HITBOX_HEIGHT = 30.f;

    // Configure sprite
    auto axeManSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    axeManSprite->setTextureRect(AXEMAN_SPRITE_REGION);
    axeManSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = axeManSprite->getLocalBounds();
    axeManSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };
    return new AxeMan(axeManSprite, hitboxes, level, stage);
}

RedSkeleton *createRedSkeleton(const sf::Vector2f &position, const size_t &level, const size_t &stage){
    // Sprite sheet coordinates and dimensions
    const sf::IntRect RED_SKELETON_SPRITE_REGION = {{157, 122}, {16, 32}};
    const float HITBOX_WIDTH = 16.f;
    const float HITBOX_HEIGHT = 32.f;

    // Configure sprite
    auto redSkeletonSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    redSkeletonSprite->setTextureRect(RED_SKELETON_SPRITE_REGION);
    redSkeletonSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = redSkeletonSprite->getLocalBounds();
    redSkeletonSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };
    return new RedSkeleton(redSkeletonSprite, hitboxes, level, stage);
}

Crow *createCrow(const sf::Vector2f &position, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect CROW_SPRITE_REGION = {{277, 75}, {16, 16}};
    const float HITBOX_WIDTH = 14.f;
    const float HITBOX_HEIGHT = 14.f;

    // Configure sprite
    auto crowSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    crowSprite->setTextureRect(CROW_SPRITE_REGION);
    crowSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = crowSprite->getLocalBounds();
    crowSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Crow(crowSprite, hitboxes, level, stage);
}

WhiteSkeleton *createWhiteSkeleton(const sf::Vector2f &position, const size_t &level, const size_t &stage){
    // Sprite sheet coordinates and dimensions
    const sf::IntRect WHITE_SKELETON_SPRITE_REGION = {{349, 75}, {16, 32}};
    const float HITBOX_WIDTH = 16.f;
    const float HITBOX_HEIGHT = 32.f;

    // Configure sprite
    auto whiteSkeletonSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    whiteSkeletonSprite->setTextureRect(WHITE_SKELETON_SPRITE_REGION);
    whiteSkeletonSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = whiteSkeletonSprite->getLocalBounds();
    whiteSkeletonSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };
    return new WhiteSkeleton(whiteSkeletonSprite, hitboxes, level, stage);
}

Hunchback *createHunchback(const sf::Vector2f &position, const size_t &level, const size_t &stage) {
    // Sprite sheet coordinates and dimensions
    const sf::IntRect placeHolder = {{0, 0}, {16, 16}};
    const float HITBOX_WIDTH = 16.f;
    const float HITBOX_HEIGHT = 16.f;

    // Configure sprite
    auto hunchbackSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    hunchbackSprite->setTextureRect(placeHolder);
    hunchbackSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = hunchbackSprite->getLocalBounds();
    hunchbackSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };
    return new Hunchback(hunchbackSprite, hitboxes, level, stage);
}