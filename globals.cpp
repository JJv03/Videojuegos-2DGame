#include "globals.h"

/* =================================================================
 * =========================== CONSTANTS ===========================
 * ================================================================= */
// NOTE 1: constexpr goes without `extern` because it's known in compile time
// NOTE 2: const goes with `extern` because it's known in exec time


// ============ GENERAL INFO / USAGE VARIABLES =============
const unsigned int gWindowWidth = 400;
const unsigned int gWindowHeight = 400;
const float gSoundVolume = 100.0f;
const float gMusicVolume = 100.0f;

const float gGameVisibleWorld_size_x = 256.f;
const float gGameVisibleWorld_size_y = 192.f;     // Considering the overlaped half-tile


// ============ IMAGE / TEXTURE / SPRITE =============
const sf::Color gColorKeyGrey = sf::Color(0x74, 0x74, 0x74);
const sf::Color gColorKeyGreen = sf::Color(0, 128, 0);


// ============ TILEMAP VARIABLES =============
const float gTileSize = 32.f;

const float gOverlapedTileSize = gTileSize/2.f;     // It's overlaped by the GUI


// ============ GUI VARIABLES =============
const float gGUI_size_x = gGameVisibleWorld_size_x;   // +1px for error margin
const float gGUI_size_y = 35.0f;
const float gGUI_position_x = 0.0f;
const float gGUI_position_y = -gGUI_size_y + gOverlapedTileSize;
const sf::Color gGUI_color = sf::Color::Black;


const unsigned int gGUI_text_size = 7;
const sf::Color gGUI_text_color = sf::Color::White;

const float gGUI_MarginFactor = 0.015f;
const float gGUI_ScorePositionXFactor = 0.015f;
const float gGUI_TimePositionXFactor = 0.46f;
const float gGUI_StagePositionXFactor = 0.76f;
const float gGUI_PlayerPositionYFactor = 10.0f;
const float gGUI_EnemyPositionYFactor = 20.0f;

const float gGUI_PlayerHpBar_position_x = 55.f;
const float gGUI_PlayerHpBar_position_y = 31.f - gGUI_size_y;
const float gGUI_BossHpBar_position_x = 55.f;
const float gGUI_BossHpBar_position_y = 40.f - gGUI_size_y;


// ============ GAME STATE VARIABLES =============
const float gMenuGS_size_x = 400.0f;
const float gMenuGS_size_y = 400.0f;
const float gMenuGS_position_x = 0.0f;
const float gMenuGS_position_y = 0.0f;

const float gGameGS_size_x = gGameVisibleWorld_size_x;
const float gGameGS_size_y = gGUI_size_y + gGameVisibleWorld_size_y - gOverlapedTileSize;
const float gGameGS_position_x = 0.0f;
const float gGameGS_position_y = gGUI_position_y;


// ============ PLAYER VARIABLES =============
const float gPlayerMovementSpeed = 60.0f;
