#pragma once
#include "olcPixelGameEngine.h"

// ============================================================================
// GAME CONFIGURATION CONSTANTS
// ============================================================================
namespace GameConfig {
    // Player Settings
    constexpr float PLAYER_SPEED = 180.0f;
    constexpr float PLAYER_RADIUS = 30.0f;
    constexpr int PLAYER_STARTING_LIVES = 3;
    constexpr float INVINCIBILITY_DURATION = 2.0f;

    // Bullet Settings
    constexpr float BULLET_RADIUS = 6.0f;
    constexpr float PLAYER_BULLET_SPEED = 350.0f;
    constexpr float ENEMY_BULLET_SPEED = 220.0f;
    constexpr float BOSS_BULLET_SPEED = 260.0f;
    constexpr float PLAYER_FIRE_COOLDOWN = 0.30f;
    constexpr float ENEMY_FIRE_COOLDOWN = 1.5f;
    constexpr float BOSS_FIRE_COOLDOWN = 1.0f;
    
    // Enemy Settings
    constexpr float ENEMY_RADIUS = 20.0f; // Default enemy radius

    // Boss Settings
    constexpr int BOSS_MAX_HP = 200;
    constexpr float BOSS_RADIUS = 60.0f;
    constexpr int BOSS_DAMAGE_PER_HIT = 5;

    // Scoring
    constexpr int SCORE_ASTEROID = 5;
    constexpr int SCORE_ENEMY = 10;
    constexpr int SCORE_BOSS_HIT = 25;

    // Level Settings
    constexpr float LEVEL1_DURATION = 25.0f;
    constexpr int LEVEL2_KILL_TARGET = 15;
    constexpr int MAX_ENEMIES_ON_SCREEN = 4;

    // Explosion Settings
    constexpr float EXPLOSION_ASTEROID_DURATION = 0.25f;
    constexpr float EXPLOSION_SHIP_DURATION = 0.35f;

    // Vector Reserve Sizes (Performance)
    constexpr size_t RESERVE_BULLETS = 50;
    constexpr size_t RESERVE_ASTEROIDS = 30;
    constexpr size_t RESERVE_ENEMIES = 10;
    constexpr size_t RESERVE_ENEMY_BULLETS = 50;
    constexpr size_t RESERVE_EXPLOSIONS = 20;
    constexpr size_t RESERVE_POWERUPS = 10;

    // Power-Up Settings
    constexpr float POWERUP_SPAWN_CHANCE = 0.15f;       // 15% chance on enemy kill
    constexpr float POWERUP_SPEED = 80.0f;              // Fall speed
    constexpr float POWERUP_RADIUS = 15.0f;
    constexpr float POWERUP_DURATION = 8.0f;            // How long effects last
    constexpr float SPEED_BOOST_MULTIPLIER = 1.5f;      // 50% faster
    constexpr float DOUBLE_SHOT_SPREAD = 15.0f;         // Pixel spread for double shot

    // Screen Shake Settings
    constexpr float SHAKE_INTENSITY_SMALL = 3.0f;
    constexpr float SHAKE_INTENSITY_MEDIUM = 6.0f;
    constexpr float SHAKE_INTENSITY_LARGE = 10.0f;
    constexpr float SHAKE_DURATION_SHORT = 0.15f;
    constexpr float SHAKE_DURATION_MEDIUM = 0.3f;

    // Difficulty Multipliers
    constexpr float DIFFICULTY_EASY_MULT = 0.7f;
    constexpr float DIFFICULTY_NORMAL_MULT = 1.0f;
    constexpr float DIFFICULTY_HARD_MULT = 1.4f;
    
    // Asteroid Settings
    constexpr float ASTEROID_RADIUS_MIN = 24.0f;
    constexpr float ASTEROID_RADIUS_MAX = 40.0f;
}

// ============================================================================
// DIFFICULTY SYSTEM
// ============================================================================
enum class Difficulty {
    EASY,
    NORMAL,
    HARD
};

// ============================================================================
// POWER-UP TYPES
// ============================================================================
enum class PowerUpType {
    DOUBLE_SHOT,    // Fires two bullets at once
    SPEED_BOOST,    // Increased player speed
    SHIELD,         // Temporary invincibility
    EXTRA_LIFE      // +1 life
};

// Inline utility function for distance squared
inline float Dist2(const olc::vf2d& a, const olc::vf2d& b) {
    olc::vf2d d = a - b;
    return d.x * d.x + d.y * d.y;
}
