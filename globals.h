#pragma once
#include <SFML/Graphics.hpp>

/* =================================================================
 * =========================== CONSTANTS ===========================
 * ================================================================= */
// NOTE 1: constexpr goes without `extern` because it's known in compile time
// NOTE 2: const goes with `extern` because it's known in exec time


// ============ GENERAL INFO / USAGE VARIABLES =============
extern const unsigned int gWindowWidth;
extern const unsigned int gWindowHeight;
// extern const float gSoundVolume;
// extern const float gMusicVolume;

extern const float gGameVisibleWorld_size_x;
extern const float gGameVisibleWorld_size_y;

extern const float gSimonFeetCollisionNewHeight;

// ============ IMAGE / TEXTURE / SPRITE =============
extern const sf::Color gColorKeyGrey;
extern const sf::Color gColorKeyGreen;


// ============ TILEMAP VARIABLES =============
extern const float gTileSize;


// ============ GUI VARIABLES =============
extern const float gGUI_size_x;
extern const float gGUI_size_y;
extern const float gGUI_position_x;
extern const float gGUI_position_y;
extern const sf::Color gGUI_color;


extern const unsigned int gGUI_text_size;
extern const sf::Color gGUI_text_color;

extern const float gGUI_MarginFactor;
extern const float gGUI_ScorePositionXFactor;
extern const float gGUI_TimePositionXFactor;
extern const float gGUI_StagePositionXFactor;
extern const float gGUI_PlayerPositionYFactor;
extern const float gGUI_EnemyPositionYFactor;
extern const float gGUI_PlayerHpBar_position_x;
extern const float gGUI_PlayerHpBar_position_y;
extern const float gGUI_BossHpBar_position_x;
extern const float gGUI_BossHpBar_position_y;


// ============ GAME STATE VARIABLES =============
extern const float gMenuGS_size_x;
extern const float gMenuGS_size_y;
extern const float gMenuGS_position_x;
extern const float gMenuGS_position_y;

extern const float gGameGS_size_x;
extern const float gGameGS_size_y;
extern const float gGameGS_position_x;
extern const float gGameGS_position_y;


// ============ PLAYER VARIABLES =============
extern const float gPlayerMovementSpeed;


// ============ CONFIGURATION VARIABLES (maybe???) =============
