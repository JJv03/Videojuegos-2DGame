#pragma once
#include <SFML/Graphics.hpp>
#include "soundManager.h"

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

extern const float gLevelTimeLimit;       // How many seconds the player has to finish a level

extern const float gLoadingTime;          // Loading starting a stage
extern const float gRevivingTime;         // Reviving time after dying


// ============ IMAGE / TEXTURE / SPRITE =============
extern const sf::Color gColorKeyGrey;
extern const sf::Color gColorKeyGreen;


// ============ TILEMAP VARIABLES =============

extern int gStartingLevel;
extern int gStartingStage;

extern const float gTileSize;

extern const float gOverlapedTileSize;  // Size of the overlap between the GUI and the tile


// ============ GUI VARIABLES =============
extern const float gGUI_size_x;
extern const float gGUI_size_y;
extern const float gGUI_position_x;
extern const float gGUI_position_y;
extern const sf::Color gGUI_color;

extern const float gGUI_subweaponBox_size_x;
extern const float gGUI_subweaponBox_size_y;
extern const float gGUI_subweaponBox_size_x;
extern const float gGUI_subweaponBox_offset_position_x;
extern const float gGUI_subweaponBox_offset_position_y;

extern const float gGUI_heartCounter_position_x;
extern const float gGUI_heartCounter_position_y;

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


// ============ ITEM VARIABLES =============
extern const float gItemGravity;


// ============ PLAYER VARIABLES =============
extern const int gWhipMaxLevel;

extern const float gSimonFeetCollisionNewHeight;    // Height above the corresponding tile at which Simon is placed after his feet are collide

extern const float gPlayerMovementSpeed;
extern const float gPlayerStairSpeed;
extern const float gPlayerGravity;
extern const float gPlayerJumpForce;

// ============ ENEMIES VARIABLES =============
extern bool bossExist;

// ============ GAME VARIABLES ============
extern SoundManager gameSoundManager;
extern bool goBack;

// ============ DEBUG =============
extern const bool gDrawHitboxes;
extern const bool gSkipAnims;