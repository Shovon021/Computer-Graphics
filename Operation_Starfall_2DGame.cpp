#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_MINIAUDIO
#include "olcPGEX_MiniAudio.h"

#include "src/GameConfig.h"
#include "src/player.h"
#include "src/asteroid.h"
#include "src/bullet.h"
#include "src/enemy.h"
#include "src/enemy_bullet.h"
#include "src/enemy_boss.h"

#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <cmath>
#include <fstream>
#include <iostream>

// ============================================================================
// GAME LOGIC
// ============================================================================
// Constants and Enums are now in src/GameConfig.h

enum class GameState {
    MENU,
    STORY,          // Intro Story
    STORY_LEVEL2,   // Pre-Level 2 Story
    STORY_LEVEL3,   // Pre-Level 3 (Final) Story
    STORY_GAMEOVER, // Mission Failed Story
    STORY_VICTORY,  // Victory/Happy Ending Story
    LEVEL_INTRO,
    PLAYING,
    GAME_OVER,
    PAUSED
};

class SpaceShooter : public olc::PixelGameEngine
{
public:
    SpaceShooter()
    {
        sAppName = "Operation Starfall";
    }

    // ============================================================================
    // DESTRUCTOR - Fix Memory Leaks
    // ============================================================================
    ~SpaceShooter() {
        // Clean up decals first (they reference sprites)
        delete decBackground;
        delete decPlayer;
        delete decAsteroid;
        delete decEnemy;
        delete decBoss;
        delete decBullet;
        delete decEnemyBullet;
        delete decBoomAsteroid;
        delete decBoomShip;

        // Then clean up sprites
        delete sprBackground;
        delete sprPlayer;
        delete sprAsteroid;
        delete sprEnemy;
        delete sprBoss;
        delete sprBullet;
        delete sprEnemyBullet;
        delete sprBoomAsteroid;
        delete sprBoomShip;

        // Clean up story decals and sprites
        for (auto* d : storyDecals) delete d;
        for (auto* s : storySprites) delete s;
        
        for (auto* d : storyL2Decals) delete d;
        for (auto* s : storyL2Sprites) delete s;
        
        for (auto* d : storyL3Decals) delete d;
        for (auto* s : storyL3Sprites) delete s;
        
        for (auto* d : storyGODecals) delete d;
        for (auto* s : storyGOSprites) delete s;
        
        for (auto* d : storyVictoryDecals) delete d;
        for (auto* s : storyVictorySprites) delete s;
    }

    // ============================================================================
    // AUDIO SYSTEM
    // ============================================================================
    olc::MiniAudio audio;
    
    // Sound IDs
    int sndShoot = -1;
    int sndExplosionSmall = -1;    // Asteroid destruction
    int sndExplosionLarge = -1;    // Ship/boss destruction
    int sndPowerUp = -1;
    int sndPlayerHit = -1;
    int sndLevelComplete = -1;
    int sndGameOver = -1;
    int sndMenuSelect = -1;
    int sndBossHit = -1;
    
    bool audioLoaded = false;

    void loadAudioFiles() {
        std::cout << "Loading audio files..." << std::endl;
        
        // Try to load audio files - these will be optional
        sndShoot = audio.LoadSound("assets/audio/shoot.wav");
        sndExplosionSmall = audio.LoadSound("assets/audio/explosion_small.wav");
        sndExplosionLarge = audio.LoadSound("assets/audio/explosion_large.wav");
        sndPowerUp = audio.LoadSound("assets/audio/powerup.wav");
        sndPlayerHit = audio.LoadSound("assets/audio/player_hit.wav");
        sndLevelComplete = audio.LoadSound("assets/audio/level_complete.wav");
        sndGameOver = audio.LoadSound("assets/audio/game_over.wav");
        sndMenuSelect = audio.LoadSound("assets/audio/menu_select.wav");
        sndBossHit = audio.LoadSound("assets/audio/boss_hit.wav");

        // Story Music (optional)
        sndStoryEpic = audio.LoadSound("assets/audio/story_epic.wav");
        sndStorySad = audio.LoadSound("assets/audio/story_sad.wav");
        sndStoryTriumph = audio.LoadSound("assets/audio/story_triumph.wav");

        // Debug output for story music loading
        std::cout << "Story Music Loading Status:" << std::endl;
        std::cout << "  - Epic: " << (sndStoryEpic >= 0 ? "OK" : "FAILED") << " (ID: " << sndStoryEpic << ")" << std::endl;
        std::cout << "  - Sad: " << (sndStorySad >= 0 ? "OK" : "FAILED") << " (ID: " << sndStorySad << ")" << std::endl;
        std::cout << "  - Triumph: " << (sndStoryTriumph >= 0 ? "OK" : "FAILED") << " (ID: " << sndStoryTriumph << ")" << std::endl;

        audioLoaded = true;
        std::cout << "Audio loading complete." << std::endl;
    }

    void playSound(int soundId, float volume = 1.0f) {
        if (audioLoaded && soundId >= 0) {
            audio.SetVolume(soundId, volume);
            audio.Play(soundId, false);  // false = no looping
        }
    }

    // ---  Explosion Struct and Vector ---
    struct Explosion {
        olc::vf2d pos;
        olc::Decal* decal;
        float timer = 0.0f;
        float maxTime = 0.25f;
        float scale = 1.0f;
    };
    std::vector<Explosion> explosions;

    // --- Power-Up Struct and Vector ---
    struct PowerUp {
        olc::vf2d pos;
        olc::vf2d vel;
        PowerUpType type;
        float r = GameConfig::POWERUP_RADIUS;
        bool alive = true;

        void Update(float dt, int screenH) {
            pos += vel * dt;
            if (pos.y - r > screenH + 20.0f) {
                alive = false;
            }
        }

        void Draw(olc::PixelGameEngine* pge) {
            if (!alive) return;
            
            // Draw colored circle based on type
            olc::Pixel color;
            std::string label;
            switch (type) {
                case PowerUpType::DOUBLE_SHOT:  color = olc::YELLOW; label = "2X"; break;
                case PowerUpType::SPEED_BOOST:  color = olc::CYAN;   label = "SP"; break;
                case PowerUpType::SHIELD:       color = olc::BLUE;   label = "SH"; break;
                case PowerUpType::EXTRA_LIFE:   color = olc::GREEN;  label = "+1"; break;
            }
            
            pge->FillCircle(pos, int(r), color);
            pge->DrawCircle(pos, int(r), olc::WHITE);
            pge->DrawString(int(pos.x - 8), int(pos.y - 4), label, olc::BLACK, 1);
        }
    };
    std::vector<PowerUp> powerups;

    // --- Sprites & decals ---
    olc::Sprite* sprBackground = nullptr;
    olc::Decal* decBackground = nullptr;

    olc::Sprite* sprPlayer = nullptr;
    olc::Decal* decPlayer = nullptr;

    olc::Sprite* sprAsteroid = nullptr;
    olc::Decal* decAsteroid = nullptr;

    olc::Sprite* sprEnemy = nullptr;
    olc::Decal* decEnemy = nullptr;

    olc::Sprite* sprBoss = nullptr;
    olc::Decal* decBoss = nullptr;

    olc::Sprite* sprBullet = nullptr;
    olc::Decal* decBullet = nullptr;

    olc::Sprite* sprEnemyBullet = nullptr; // New
    olc::Decal* decEnemyBullet = nullptr; // New

    olc::Sprite* sprBoomAsteroid = nullptr;
    olc::Decal* decBoomAsteroid = nullptr;

    olc::Sprite* sprBoomShip = nullptr;
    olc::Decal* decBoomShip = nullptr;

    // Story Sprites & Decals
    std::vector<olc::Sprite*> storySprites;
    std::vector<olc::Decal*> storyDecals;
    int storyIndex = 0;
    std::vector<std::string> storyText;

    // Story Level 2 Sprites & Decals
    std::vector<olc::Sprite*> storyL2Sprites;
    std::vector<olc::Decal*> storyL2Decals;
    int storyL2Index = 0;
    std::vector<std::string> storyL2Text;

    // Story Level 3 Sprites & Decals
    std::vector<olc::Sprite*> storyL3Sprites;
    std::vector<olc::Decal*> storyL3Decals;
    int storyL3Index = 0;
    std::vector<std::string> storyL3Text;

    // Story Game Over (Mission Failed) Sprites & Decals
    std::vector<olc::Sprite*> storyGOSprites;
    std::vector<olc::Decal*> storyGODecals;
    int storyGOIndex = 0;
    std::vector<std::string> storyGOText;

    // Story Victory (Happy Ending) Sprites & Decals
    std::vector<olc::Sprite*> storyVictorySprites;
    std::vector<olc::Decal*> storyVictoryDecals;
    int storyVictoryIndex = 0;
    std::vector<std::string> storyVictoryText;

    // Story Enhancement Variables
    float storyTypewriterTimer = 0.0f;   // For typewriter effect (chars revealed)
    float storyFadeTimer = 0.0f;         // For fade transitions
    float storySlideTimer = 0.0f;        // For auto-advance
    float storyParallaxTime = 0.0f;      // For parallax motion
    float storySkipTimer = 0.0f;         // For skip detection (ESC hold)
    bool storyFadingIn = true;           // Fade state
    bool storyMusicPlaying = false;      // Track if story music is currently playing
    int currentStoryMusic = -1;          // Currently playing story music ID

    // Story Music IDs
    int sndStoryEpic = -1;
    int sndStorySad = -1;
    int sndStoryTriumph = -1;

    // Background scroll
    float bgOffset = 0.0f;

    // --- Main Core Parts Objects ---
    Player player;
    std::vector<Asteroid> asteroids;
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    Boss boss;
    std::vector<EnemyBullet> enemyBullets;

    // Random
    std::mt19937 rng{ std::random_device{}() };

    // --- Difficulty System ---
    Difficulty difficulty = Difficulty::NORMAL;

    float getDifficultyMultiplier() const {
        switch (difficulty) {
            case Difficulty::EASY:   return GameConfig::DIFFICULTY_EASY_MULT;
            case Difficulty::NORMAL: return GameConfig::DIFFICULTY_NORMAL_MULT;
            case Difficulty::HARD:   return GameConfig::DIFFICULTY_HARD_MULT;
        }
        return 1.0f;
    }

    std::string getDifficultyName() const {
        switch (difficulty) {
            case Difficulty::EASY:   return "EASY";
            case Difficulty::NORMAL: return "NORMAL";
            case Difficulty::HARD:   return "HARD";
        }
        return "NORMAL";
    }

    // --- Screen Shake System ---
    float shakeTimer = 0.0f;
    float shakeIntensity = 0.0f;

    void triggerScreenShake(float intensity, float duration) {
        shakeIntensity = intensity;
        shakeTimer = duration;
    }

    olc::vf2d getShakeOffset() {
        if (shakeTimer <= 0.0f) return { 0.0f, 0.0f };
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        return { dist(rng) * shakeIntensity, dist(rng) * shakeIntensity };
    }

    // --- Active Power-Up Timers ---
    float doubleShotTimer = 0.0f;
    float speedBoostTimer = 0.0f;
    float shieldTimer = 0.0f;

    bool hasDoubleShot() const { return doubleShotTimer > 0.0f; }
    bool hasSpeedBoost() const { return speedBoostTimer > 0.0f; }
    bool hasShield() const { return shieldTimer > 0.0f; }

    // ============================================================================
    // STORY RENDERING HELPER (with all enhancements)
    // ============================================================================
    enum class StoryMood { NEUTRAL, SAD, TRIUMPHANT };

    void resetStorySlideState() {
        storyTypewriterTimer = 0.0f;
        storyFadeTimer = 0.0f;
        storySlideTimer = 0.0f;
        storyFadingIn = true;
    }

    // Returns true if player wants to advance to next slide
    bool renderStorySlide(olc::Decal* decal, const std::string& text, StoryMood mood, float dt) {
        const float FADE_DURATION = 0.5f;
        const float AUTO_ADVANCE_TIME = 8.0f;
        const float TYPEWRITER_SPEED = 40.0f;  // chars per second
        const float PARALLAX_AMPLITUDE = 8.0f;
        const float PARALLAX_SPEED = 0.4f;
        const float SKIP_HOLD_TIME = 1.5f;

        // Update timers
        storySlideTimer += dt;
        storyTypewriterTimer += dt;
        storyParallaxTime += dt;
        storyFadeTimer += dt;

        // Clear screen
        Clear(olc::BLACK);

        // --- 1. PARALLAX IMAGE RENDERING ---
        // Set normal decal mode for proper rendering
        SetDecalMode(olc::DecalMode::NORMAL);
        
        if (decal && decal->sprite) {
            // Scale image to FILL the area above text box (may crop sides)
            float availableHeight = float(ScreenHeight() - 80);
            float scaleX = float(ScreenWidth()) / float(decal->sprite->width);
            float scaleY = availableHeight / float(decal->sprite->height);
            float scale = std::max(scaleX, scaleY);  // Fill area (may crop)

            float drawW = decal->sprite->width * scale;
            float drawH = decal->sprite->height * scale;
            float offX = (ScreenWidth() - drawW) / 2.0f;  // Center horizontally
            float offY = (availableHeight - drawH) / 2.0f;  // Center in available area

            // Parallax offset (gentle horizontal sway)
            float parallaxX = sin(storyParallaxTime * PARALLAX_SPEED * 2.0f * 3.14159f) * PARALLAX_AMPLITUDE;

            DrawDecal({ offX + parallaxX, offY }, decal, { scale, scale });
        }

        // --- 2. TEXT BOX (always at bottom) ---
        olc::Pixel textColor, promptColor;
        switch (mood) {
            case StoryMood::SAD:
                textColor = olc::Pixel(200, 150, 150);  // Light red/pink
                promptColor = olc::RED;
                break;
            case StoryMood::TRIUMPHANT:
                textColor = olc::Pixel(150, 255, 150);  // Light green
                promptColor = olc::GREEN;
                break;
            default:
                textColor = olc::CYAN;  // Bright cyan for visibility
                promptColor = olc::YELLOW;
        }

        // Dark background for text area
        FillRect(0, ScreenHeight() - 80, ScreenWidth(), 80, olc::Pixel(20, 20, 40));
        DrawLine(0, ScreenHeight() - 80, ScreenWidth(), ScreenHeight() - 80, olc::DARK_GREY);

        // Typewriter effect
        int charsToShow = int(storyTypewriterTimer * TYPEWRITER_SPEED);
        std::string displayText = text.substr(0, std::min((size_t)charsToShow, text.size()));
        
        // DEBUG: Print to console every 30 frames
        static int debugFrameCount = 0;
        if (debugFrameCount++ % 30 == 0) {
            std::cout << "Timer=" << storyTypewriterTimer << " chars=" << charsToShow << " text='" << displayText.substr(0, 30) << "'" << std::endl;
        }
        
        // Draw the text with scale 1 for readability
        DrawString(15, ScreenHeight() - 65, displayText, textColor, 1);

        // --- 3. FADE IN/OUT OVERLAY ---
        int fadeAlpha = 0;
        if (storyFadingIn && storyFadeTimer < FADE_DURATION) {
            fadeAlpha = int(255.0f * (1.0f - storyFadeTimer / FADE_DURATION));
        }
        if (fadeAlpha > 0) {
            FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::Pixel(0, 0, 0, fadeAlpha));
        }

        // --- 4. SKIP PROMPT (ESC) ---
        if (GetKey(olc::Key::ESCAPE).bHeld) {
            storySkipTimer += dt;
            float skipProgress = std::min(storySkipTimer / SKIP_HOLD_TIME, 1.0f);
            
            // Draw skip bar
            int barW = 100;
            int barH = 8;
            int barX = 10;
            int barY = 10;
            DrawRect(barX, barY, barW, barH, olc::WHITE);
            FillRect(barX + 1, barY + 1, int((barW - 2) * skipProgress), barH - 2, olc::YELLOW);
            DrawString(barX + barW + 10, barY, "Hold ESC to Skip", olc::GREY, 1);
            
            if (storySkipTimer >= SKIP_HOLD_TIME) {
                return true;  // Signal to skip entire story
            }
        } else {
            storySkipTimer = 0.0f;
        }

        // --- 5. AUTO-ADVANCE TIMER ---
        float timeLeft = AUTO_ADVANCE_TIME - storySlideTimer;
        if (timeLeft > 0) {
            std::string timerStr = "Auto: " + std::to_string(int(timeLeft) + 1) + "s";
            DrawString(ScreenWidth() - 70, 10, timerStr, olc::DARK_GREY, 1);
        }

        // --- 6. BLINKING ADVANCE PROMPT ---
        if (fmod(storySlideTimer, 1.0f) > 0.4f) {
            DrawString(ScreenWidth() - 110, ScreenHeight() - 25, "PRESS SPACE >>", promptColor, 1);
        }

        // --- 7. CHECK FOR ADVANCE ---
        bool advance = false;
        if (GetKey(olc::Key::ENTER).bPressed || GetKey(olc::Key::SPACE).bPressed) {
            advance = true;
        }
        if (storySlideTimer >= AUTO_ADVANCE_TIME) {
            advance = true;
        }

        return advance;
    }

    void startStoryMusic(StoryMood mood) {
        // Don't restart if already playing
        if (storyMusicPlaying) return;
        
        // Stop any current story music
        stopStoryMusic();

        // Play appropriate music with volume
        int soundToPlay = -1;
        switch (mood) {
            case StoryMood::SAD:
                soundToPlay = sndStorySad;
                break;
            case StoryMood::TRIUMPHANT:
                soundToPlay = sndStoryTriumph;
                break;
            default:
                soundToPlay = sndStoryEpic;
        }

        if (soundToPlay != -1) {
            // Play with looping enabled and set volume
            audio.SetVolume(soundToPlay, 0.6f);
            audio.Play(soundToPlay, true); 
            currentStoryMusic = soundToPlay;
            storyMusicPlaying = true;
            std::cout << "Playing story music: " << soundToPlay << std::endl;
        } else {
            std::cout << "Story music not loaded!" << std::endl;
        }
    }

    void stopStoryMusic() {
        if (currentStoryMusic != -1) {
            // First pause, then stop to ensure it really stops
            audio.Pause(currentStoryMusic);
            audio.Stop(currentStoryMusic);
            std::cout << "Stopped story music ID: " << currentStoryMusic << std::endl;
            currentStoryMusic = -1;
            storyMusicPlaying = false;
        }
    }
    
    // Force stop ALL story music (call when entering gameplay)
    void forceStopAllStoryMusic() {
        if (sndStoryEpic != -1) audio.Stop(sndStoryEpic);
        if (sndStorySad != -1) audio.Stop(sndStorySad);
        if (sndStoryTriumph != -1) audio.Stop(sndStoryTriumph);
        currentStoryMusic = -1;
        storyMusicPlaying = false;
    }

    // Timer & Level
    GameState state = GameState::MENU;
    int currentLevel = 0;

    float introTimer = 0.0f;
    float levelTime = 0.0f;

    // Spawning timers
    float spawnTimer = 0.0f;
    float spawnRate = 0.5f;
    float fireCoolDown = GameConfig::PLAYER_FIRE_COOLDOWN;
    float fireTimer = 0.0f;
    float enemySpawnTimer = 0.0f;
    float enemySpawnRate = 2.0f;
    int maxEnemies = GameConfig::MAX_ENEMIES_ON_SCREEN;
    float enemyFireTimer = 0.0f;
    float enemyFireCooldown = GameConfig::ENEMY_FIRE_COOLDOWN;
    float bossFireTimer = 0.0f;
    float bossFireCooldown = GameConfig::BOSS_FIRE_COOLDOWN;

    // Stats
    int score = 0;
    int hits = 0;
    bool wins = false;
    int enemiesKilled = 0;
    int total_enemy_spawn = 0;

    // High Score System
    int highScore = 0;
    const std::string HIGH_SCORE_FILE = "highscore.txt";

    // ============================================================================
    // HIGH SCORE FUNCTIONS
    // ============================================================================
    void loadHighScore() {
        std::ifstream file(HIGH_SCORE_FILE);
        if (file.is_open()) {
            file >> highScore;
            file.close();
            std::cout << "Loaded high score: " << highScore << std::endl;
        }
        else {
            highScore = 0;
            std::cout << "No high score file found. Starting fresh." << std::endl;
        }
    }

    void saveHighScore() {
        if (score > highScore) {
            highScore = score;
            std::ofstream file(HIGH_SCORE_FILE);
            if (file.is_open()) {
                file << highScore;
                file.close();
                std::cout << "New high score saved: " << highScore << std::endl;
            }
        }
    }

    // ============================================================================
    // HELPER: Load sprite with validation
    // ============================================================================
    bool loadSprite(olc::Sprite*& spr, olc::Decal*& dec, const std::string& path) {
        spr = new olc::Sprite(path);
        if (spr->width == 0 || spr->height == 0) {
            std::cerr << "ERROR: Failed to load sprite: " << path << std::endl;
            return false;
        }
        dec = new olc::Decal(spr);
        std::cout << "Loaded: " << path << " (" << spr->width << "x" << spr->height << ")" << std::endl;
        return true;
    }

    // ============================================================================
    // HELPER: Handle player taking damage (reduces code duplication)
    // ============================================================================
    void handlePlayerHit() {
        // Shield power-up blocks damage
        if (hasShield()) return;
        
        if (player.invincibleTimer > 0.0f) return;  // Already invincible

        hits++;
        player.lives--;
        player.invincibleTimer = GameConfig::INVINCIBILITY_DURATION;
        
        // Play player hit sound
        playSound(sndPlayerHit, 0.7f);
        
        // Trigger screen shake on hit
        triggerScreenShake(GameConfig::SHAKE_INTENSITY_MEDIUM, GameConfig::SHAKE_DURATION_MEDIUM);

        if (player.lives <= 0) {
            spawnExplosion(player.pos, decBoomShip, GameConfig::EXPLOSION_SHIP_DURATION,
                (player.r * 2.0f) / sprBoomShip->width);
            triggerScreenShake(GameConfig::SHAKE_INTENSITY_LARGE, GameConfig::SHAKE_DURATION_MEDIUM);
            playSound(sndGameOver, 1.0f);
            // Go to Game Over Story instead of directly to Game Over
            state = GameState::STORY_GAMEOVER;
            storyGOIndex = 0;
            resetStorySlideState();
            storyMusicPlaying = false;
            wins = false;
            saveHighScore();
        }
    }

    // ============================================================================
    // POWER-UP SYSTEM
    // ============================================================================
    void spawnPowerUp(const olc::vf2d& pos) {
        // Random chance to spawn
        std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
        if (chanceDist(rng) > GameConfig::POWERUP_SPAWN_CHANCE) return;

        // Random power-up type
        std::uniform_int_distribution<int> typeDist(0, 3);
        PowerUpType type = static_cast<PowerUpType>(typeDist(rng));

        PowerUp p;
        p.pos = pos;
        p.vel = { 0.0f, GameConfig::POWERUP_SPEED };
        p.type = type;
        p.r = GameConfig::POWERUP_RADIUS;
        p.alive = true;

        powerups.push_back(p);
    }

    void applyPowerUp(PowerUpType type) {
        // Play power-up sound
        playSound(sndPowerUp, 0.8f);
        
        switch (type) {
            case PowerUpType::DOUBLE_SHOT:
                doubleShotTimer = GameConfig::POWERUP_DURATION;
                std::cout << "Power-up: DOUBLE SHOT!" << std::endl;
                break;
            case PowerUpType::SPEED_BOOST:
                speedBoostTimer = GameConfig::POWERUP_DURATION;
                std::cout << "Power-up: SPEED BOOST!" << std::endl;
                break;
            case PowerUpType::SHIELD:
                shieldTimer = GameConfig::POWERUP_DURATION;
                std::cout << "Power-up: SHIELD!" << std::endl;
                break;
            case PowerUpType::EXTRA_LIFE:
                player.lives++;
                std::cout << "Power-up: EXTRA LIFE! Lives: " << player.lives << std::endl;
                break;
        }
    }

    void updatePowerUpTimers(float dt) {
        if (doubleShotTimer > 0.0f) doubleShotTimer -= dt;
        if (speedBoostTimer > 0.0f) speedBoostTimer -= dt;
        if (shieldTimer > 0.0f) shieldTimer -= dt;
        if (shakeTimer > 0.0f) shakeTimer -= dt;
    }

    void spawnEnemy() {
        std::uniform_real_distribution<float> xDist(40.0f, float(ScreenWidth() - 40.0f));
        std::uniform_real_distribution<float> vyDist(80.0f, 130.0f);
        std::uniform_real_distribution<float> vxDist(-40.0f, 40.0f);

        Enemy e;
        e.pos = { xDist(rng), -40.0f };
        e.vel = { vxDist(rng), vyDist(rng) };
        e.r = 20.0f;
        e.alive = true;
        e.inArena = false;
        e.decal = decEnemy;

        enemies.push_back(e);
    }

    void spawnAsteroid() {
        std::uniform_real_distribution<float> xDist(10.0f, float(ScreenWidth()) - 10.0f);
        std::uniform_real_distribution<float> vyDist(80.0f, 150.0f);
        std::uniform_real_distribution<float> vxDist(-20.0f, 20.0f);
        std::uniform_real_distribution<float> rDist(24.0f, 40.0f);

        Asteroid a;
        a.pos = { xDist(rng), -30.0f };
        a.vel = { vxDist(rng), vyDist(rng) };
        a.r = rDist(rng);
        a.alive = true;
        a.decal = decAsteroid;

        asteroids.push_back(a);
    }

    void spawnBullet(const olc::vf2d& startpos) {
        Bullet b;
        b.pos = startpos;
        b.vel = { 0.0f, -GameConfig::PLAYER_BULLET_SPEED };
        b.r = 4.0f;
        b.alive = true;
        b.decal = decBullet;

        bullets.push_back(b);
    }

    // Spawns player bullets (handles double shot power-up)
    void spawnPlayerBullets() {
        olc::vf2d basePos = player.pos + olc::vf2d{ 0.0f, -player.r };
        
        if (hasDoubleShot()) {
            // Spawn two bullets with spread
            spawnBullet(basePos + olc::vf2d{ -GameConfig::DOUBLE_SHOT_SPREAD, 0.0f });
            spawnBullet(basePos + olc::vf2d{ GameConfig::DOUBLE_SHOT_SPREAD, 0.0f });
        } else {
            spawnBullet(basePos);
        }
        
        playSound(sndShoot, 0.3f);  // Lower volume for frequent shooting
    }

    void spawnEnemyBullet(const olc::vf2d& startPos) {
        EnemyBullet eb;
        eb.pos = startPos;
        eb.vel = { 0.0f, 220.0f };
        eb.r = 4.0f;
        eb.alive = true;
        eb.decal = decBullet;
        enemyBullets.push_back(eb);
    }

    void spawnBossBullets() {
        if (!boss.alive) return;

        olc::vf2d leftmuzz = boss.pos + olc::vf2d{ -30.0f, boss.r * 0.5f };
        olc::vf2d rightmuzz = boss.pos + olc::vf2d{ 30.0f, boss.r * 0.5f };

        EnemyBullet bl;
        bl.pos = leftmuzz;
        bl.vel = { 0.0f, 260.0f };
        bl.r = 4.0f;
        bl.alive = true;
        bl.decal = decBullet;

        EnemyBullet br = bl;
        br.pos = rightmuzz;

        enemyBullets.push_back(bl);
        enemyBullets.push_back(br);
    }

    void spawnExplosion(const olc::vf2d& pos, olc::Decal* decal, float maxTime, float scale = 1.0f) {
        Explosion e;
        e.pos = pos;
        e.decal = decal;
        e.maxTime = maxTime;
        e.timer = maxTime;
        e.scale = scale;
        explosions.push_back(e);
    }

    bool OnUserCreate() override
    {
        std::cout << "=== Operation Starfall Initializing ===" << std::endl;

        // Reserve vector capacity for performance
        bullets.reserve(GameConfig::RESERVE_BULLETS);
        asteroids.reserve(GameConfig::RESERVE_ASTEROIDS);
        enemies.reserve(GameConfig::RESERVE_ENEMIES);
        enemyBullets.reserve(GameConfig::RESERVE_ENEMY_BULLETS);
        explosions.reserve(GameConfig::RESERVE_EXPLOSIONS);
        powerups.reserve(GameConfig::RESERVE_POWERUPS);

        // Load all sprites with validation
      // Load Sprites
    auto loadSprite = [&](std::string path) {
        olc::Sprite* s = new olc::Sprite(path);
        if (s->width == 0) {
            std::cout << "Failed to load: " << path << std::endl;
        }
        return s;
    };

    sprBackground = loadSprite("assets/sprites/bg_space.png");
    sprPlayer = loadSprite("assets/sprites/player_ship.png");
    sprAsteroid = loadSprite("assets/sprites/asteroid.png");
    sprBullet = loadSprite("assets/sprites/bullet.png");
    sprEnemy = loadSprite("assets/sprites/enemy_ship.png");
    sprEnemyBullet = loadSprite("assets/sprites/bullet.png");  // Reuse bullet sprite
    sprBoomAsteroid = loadSprite("assets/sprites/boom_asteroid.png");
    sprBoomShip = loadSprite("assets/sprites/boom_ship.png");
    sprBoss = loadSprite("assets/sprites/boss_ship.png");

    decBackground = new olc::Decal(sprBackground);
    decPlayer = new olc::Decal(sprPlayer);
    decAsteroid = new olc::Decal(sprAsteroid);
    decBullet = new olc::Decal(sprBullet);
    decEnemy = new olc::Decal(sprEnemy);
    decEnemyBullet = new olc::Decal(sprEnemyBullet);
    decBoomAsteroid = new olc::Decal(sprBoomAsteroid);
    decBoomShip = new olc::Decal(sprBoomShip);
    decBoss = new olc::Decal(sprBoss);

    // Load Story Sprites
    std::string storyFiles[] = {
        "assets/sprites/story_1.jpg",
        "assets/sprites/story_2.jpg",
        "assets/sprites/story_3.jpg",
        "assets/sprites/story_4.png",
        "assets/sprites/story_5.png"
    };

    for (const auto& file : storyFiles) {
        olc::Sprite* s = loadSprite(file);
        storySprites.push_back(s);
        storyDecals.push_back(new olc::Decal(s));
    }

    // Story Text
    storyText = {
        "The Alien threat began with a calculated gaze upon Earth...\nTarget: Dhaka.",
        "Our Satellite 'Bangabandhu Deep Space Sentinel-1'\ndetected the incoming invasion fleet.",
        "Emergency Protocol Initiated.\nWorld Leaders gathered to formulate a pre-emptive strike.",
        "Operation Starfall Launch Initiated.\nThe Bangladesh Space Force deploys the flagship.",
        "Battle Stations!\nOur forces engage the enemy to defend our home!"
    };

    // Load Level 2 Story Sprites
    std::string storyL2Files[] = {
        "assets/sprites/story_lvl2_1.jpg",
        "assets/sprites/story_lvl2_2.jpg",
        "assets/sprites/story_lvl2_3.png"
    };

    for (const auto& file : storyL2Files) {
        olc::Sprite* s = loadSprite(file);
        storyL2Sprites.push_back(s);
        storyL2Decals.push_back(new olc::Decal(s));
    }

    // Level 2 Story Text
    storyL2Text = {
        "The Aliens are losing patience!\nTheir commander orders the swarm to attack!",
        "Humanity fights back!\nOur pilots regroup for the counter-offensive.",
        "Dogfight in Deep Space!\nEngaging enemy fighter squadrons."
    };

    // Load Level 3 Story Sprites
    std::string storyL3Files[] = {
        "assets/sprites/story_lvl3_1.png",
        "assets/sprites/story_lvl3_2.png",
        "assets/sprites/story_lvl3_3.jpg"
    };

    for (const auto& file : storyL3Files) {
        olc::Sprite* s = loadSprite(file);
        storyL3Sprites.push_back(s);
        storyL3Decals.push_back(new olc::Decal(s));
    }

    // Level 3 Story Text
    storyL3Text = {
        "The Alien Queen is furious at her fleet's failure!\nShe commands the mothership to engage.",
        "Warning: Massive Energy Signature Detected!\nThe Boss Ship is approaching.",
        "The Final Battle Begins!\nDefend Earth at all costs!"
    };

    // Load Game Over Story Sprites
    std::string storyGOFiles[] = {
        "assets/sprites/story_gameover_1.jpg",
        "assets/sprites/story_gameover_2.jpg",
        "assets/sprites/story_gameover_3.jpg"
    };

    for (const auto& file : storyGOFiles) {
        olc::Sprite* s = loadSprite(file);
        storyGOSprites.push_back(s);
        storyGODecals.push_back(new olc::Decal(s));
    }

    // Game Over Story Text
    storyGOText = {
        "Our flagship is destroyed!\nThe alien fleet overwhelms our defenses.",
        "With no one left to stop them...\nThe aliens unleash their fury upon Earth.",
        "Earth falls silent...\nHumanity's last hope is gone."
    };

    // Load Victory Story Sprites
    std::string storyVictoryFiles[] = {
        "assets/sprites/story_victory_1.jpg",
        "assets/sprites/story_victory_2.jpg",
        "assets/sprites/story_victory_3.jpg",
        "assets/sprites/story_victory_4.jpg"
    };

    for (const auto& file : storyVictoryFiles) {
        olc::Sprite* s = loadSprite(file);
        storyVictorySprites.push_back(s);
        storyVictoryDecals.push_back(new olc::Decal(s));
    }

    // Victory Story Text
    storyVictoryText = {
        "The Alien Mothership is destroyed!\nHumanity's counterattack succeeds!",
        "Our forces push forward and obliterate\nthe alien homeworld!",
        "Our heroes return home to a hero's welcome.\nLeaders embrace the saviors of Earth!",
        "The people of Earth celebrate VICTORY!\nBangladesh leads the way to peace!"
    };

        // Load high score
        loadHighScore();

        // Initialize audio system
        loadAudioFiles();

        std::cout << "=== All assets loaded successfully! ===" << std::endl;

        state = GameState::MENU;
        return true;
    }

    void ResetGame() {
        score = 0;
        hits = 0;
        player.lives = GameConfig::PLAYER_STARTING_LIVES;
        enemiesKilled = 0;
        total_enemy_spawn = 0;

        asteroids.clear();
        bullets.clear();
        enemies.clear();
        enemyBullets.clear();
        powerups.clear();
        
        // Reset power-up timers
        doubleShotTimer = 0.0f;
        speedBoostTimer = 0.0f;
        shieldTimer = 0.0f;
        shakeTimer = 0.0f;

        enemySpawnTimer = 0.0f;
        enemyFireTimer = 0.0f;
        bossFireTimer = 0.0f;

        boss.hp = boss.maxHp;
        wins = false;
        boss.alive = false;

        startLevel(1);
    }

    void startLevel(int lvl) {
        currentLevel = lvl;
        levelTime = 0.0f;
        spawnTimer = 0.0f;
        enemySpawnTimer = 0.0f;
        enemyFireTimer = 0.0f;
        enemiesKilled = 0;
        total_enemy_spawn = 0;

        bullets.clear();
        asteroids.clear();
        enemies.clear();
        enemyBullets.clear();

        if (currentLevel == 1) {
            spawnRate = 0.5f;
        }
        else if (currentLevel == 2) {
            spawnRate = 0.7f;
            enemySpawnRate = 2.0f;
        }
        else if (currentLevel == 3) {
            spawnRate = 0.9f;
            enemySpawnRate = 2.5f;

            boss.Reset({ ScreenWidth() / 2.0f, -60.0f });
            boss.decal = decBoss;

            bossFireCooldown = 1.2f;
            bossFireTimer = 1.0f;
        }

        player.decal = decPlayer;
        player.Reset({ ScreenWidth() / 2.0f, ScreenHeight() - 60.0f });
    }

    void updateCurrentLevel(float dt) {
        levelTime += dt;

        // Update power-up and shake timers
        updatePowerUpTimers(dt);

        // Player update with speed boost
        float originalSpeed = player.speed;
        if (hasSpeedBoost()) {
            player.speed = GameConfig::PLAYER_SPEED * GameConfig::SPEED_BOOST_MULTIPLIER;
        } else {
            player.speed = GameConfig::PLAYER_SPEED;
        }
        player.Update(this, dt);
        player.speed = originalSpeed; // Restore

        // Auto-shooting (now uses spawnPlayerBullets for double shot)
        fireTimer -= dt;
        if (fireTimer <= 0.0f) {
            spawnPlayerBullets();
            fireTimer = fireCoolDown;
        }

        // Spawn asteroids (with difficulty multiplier)
        float diffMult = getDifficultyMultiplier();
        spawnTimer -= dt;
        if (spawnTimer <= 0.0f) {
            spawnAsteroid();
            spawnTimer = spawnRate / diffMult;  // Faster spawns on harder difficulty
        }

        // Update power-ups
        for (auto& p : powerups) {
            if (p.alive) p.Update(dt, ScreenHeight());
        }

        // Spawn enemies (Level 2 and 3)
        if (currentLevel == 2 || currentLevel == 3) {
            enemySpawnTimer -= dt;
            if (enemySpawnTimer <= 0.0f) {
                int aliveEnemies = 0;
                for (auto& e : enemies) {
                    if (e.alive) aliveEnemies++;
                }

                bool canSpawn = true;
                if (currentLevel == 2 && total_enemy_spawn >= GameConfig::LEVEL2_KILL_TARGET)
                    canSpawn = false;

                if (canSpawn && aliveEnemies < maxEnemies) {
                    total_enemy_spawn++;
                    spawnEnemy();
                }
                enemySpawnTimer = enemySpawnRate;
            }
        }

        // Update bullets
        for (auto& b : bullets) {
            if (b.alive) b.Update(dt);
        }

        // Update asteroids
        for (auto& a : asteroids) {
            if (a.alive) a.Update(dt, ScreenHeight());
        }

        // Update enemies 
        for (auto& e : enemies) {
            if (e.alive) e.Update(dt, ScreenWidth(), ScreenHeight());
        }

        // Enemy shooting
        if (currentLevel == 2 || currentLevel == 3) {
            enemyFireTimer -= dt;
            if (enemyFireTimer <= 0.0f) {
                for (auto& e : enemies) {
                    if (!e.alive) continue;
                    olc::vf2d muz = e.pos + olc::vf2d{ 0.0f, e.r };
                    spawnEnemyBullet(muz);
                }
                enemyFireTimer = enemyFireCooldown;
            }
        }

        // Update enemy bullets
        for (auto& eb : enemyBullets) {
            if (eb.alive) eb.Update(dt, ScreenHeight());
        }
        

        // Boss update and shooting
        if (currentLevel == 3 && boss.alive) {
            boss.Update(dt, ScreenWidth());

            bossFireTimer -= dt;
            if (bossFireTimer <= 0.0f) {
                spawnBossBullets();
                bossFireTimer = bossFireCooldown;
            }
        }

        // Update Explosions
        for (auto& exp : explosions) {
            exp.timer -= dt;
        }

        // ===== COLLISION DETECTION =====

        // Asteroid vs bullets
        for (auto& b : bullets) {
            if (!b.alive) continue;
            for (auto& a : asteroids) {
                if (!a.alive) continue;
                float hitR = b.r + a.r;
                if (Dist2(b.pos, a.pos) <= hitR * hitR) {
                    b.alive = false;
                    spawnExplosion(a.pos, decBoomAsteroid, GameConfig::EXPLOSION_ASTEROID_DURATION, 
                                   a.r * 2.0f / sprBoomAsteroid->width);
                    a.alive = false;
                    score += GameConfig::SCORE_ASTEROID;
                    playSound(sndExplosionSmall, 0.4f);
                    break;
                }
            }
        }

        // Enemy vs bullets
        for (auto& b : bullets) {
            if (!b.alive) continue;
            for (auto& e : enemies) {
                if (!e.alive) continue;
                float hitR = b.r + e.r;
                if (Dist2(b.pos, e.pos) <= hitR * hitR) {
                    b.alive = false;
                    e.alive = false;
                    spawnExplosion(e.pos, decBoomShip, GameConfig::EXPLOSION_SHIP_DURATION, 
                                   (e.r * 2.0f) / sprBoomShip->width);
                    score += GameConfig::SCORE_ENEMY;
                    enemiesKilled += 1;
                    spawnPowerUp(e.pos);  // Chance to spawn power-up
                    playSound(sndExplosionLarge, 0.6f);
                    break;
                }
            }
        }

        // Power-up vs player
        for (auto& p : powerups) {
            if (!p.alive) continue;
            float hitR = p.r + player.r;
            if (Dist2(p.pos, player.pos) <= hitR * hitR) {
                p.alive = false;
                applyPowerUp(p.type);
            }
        }

        // Asteroid vs player
        for (auto& a : asteroids) {
            if (!a.alive) continue;
            float hitR = a.r + player.r;
            if (Dist2(a.pos, player.pos) <= hitR * hitR) {
                a.alive = false;
                spawnExplosion(a.pos, decBoomAsteroid, GameConfig::EXPLOSION_ASTEROID_DURATION, 
                               a.r * 2.0f / sprBoomAsteroid->width);
                handlePlayerHit();
            }
        }

        // Enemy vs player
        for (auto& e : enemies) {
            if (!e.alive) continue;
            float hitR = e.r + player.r;
            if (Dist2(e.pos, player.pos) <= hitR * hitR) {
                e.alive = false;
                spawnExplosion(e.pos, decBoomShip, GameConfig::EXPLOSION_SHIP_DURATION, 
                               (e.r * 2.0f) / sprBoomShip->width);
                handlePlayerHit();
            }
        }

        // Enemy bullets vs player
        for (auto& eb : enemyBullets) {
            if (!eb.alive) continue;
            float hitR = eb.r + player.r;
            if (Dist2(eb.pos, player.pos) <= hitR * hitR) {
                eb.alive = false;
                handlePlayerHit();
            }
        }

        // Player bullets vs boss
        if (currentLevel == 3 && boss.alive) {
            for (auto& b : bullets) {
                if (!b.alive) continue;
                float hitR = b.r + boss.r;
                if (Dist2(b.pos, boss.pos) <= hitR * hitR) {
                    b.alive = false;
                    boss.hp -= GameConfig::BOSS_DAMAGE_PER_HIT;
                    score += GameConfig::SCORE_BOSS_HIT;
                    triggerScreenShake(GameConfig::SHAKE_INTENSITY_SMALL, GameConfig::SHAKE_DURATION_SHORT);
                    playSound(sndBossHit, 0.4f);

                    if (boss.hp <= 0) {
                        boss.hp = 0;
                        boss.alive = false;
                        spawnExplosion(boss.pos, decBoomShip, GameConfig::EXPLOSION_SHIP_DURATION, 
                                       (boss.r * 2.0f) / sprBoomShip->width);
                        triggerScreenShake(GameConfig::SHAKE_INTENSITY_LARGE, GameConfig::SHAKE_DURATION_MEDIUM);
                        playSound(sndExplosionLarge, 1.0f);
                    }
                    break;
                }
            }
        }

        // Boss vs player
        if (currentLevel == 3 && boss.alive) {
            float hitR = boss.r + player.r;
            if (Dist2(boss.pos, player.pos) <= hitR * hitR) {
                handlePlayerHit();
            }
        }

        // Clean up dead objects
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                [](const Bullet& b) {return !b.alive; }),
            bullets.end()
        );

        asteroids.erase(
            std::remove_if(asteroids.begin(), asteroids.end(),
                [](const Asteroid& b) {return !b.alive; }),
            asteroids.end()
        );

        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [](const Enemy& e) { return !e.alive; }),
            enemies.end()
        );

        enemyBullets.erase(
            std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                [](const EnemyBullet& eb) { return !eb.alive; }),
            enemyBullets.end()
        );
        explosions.erase(
            std::remove_if(explosions.begin(), explosions.end(),
                [](const Explosion& exp) {return exp.timer <= 0.0f; }),
            explosions.end()
        );
        powerups.erase(
            std::remove_if(powerups.begin(), powerups.end(),
                [](const PowerUp& p) { return !p.alive; }),
            powerups.end()
        );
    }

    bool OnUserUpdate(float dt) override
    {
        Clear(olc::BLACK);

        switch (state)
        {
        case GameState::MENU:
        {
            // Clear screen
            Clear(olc::BLACK);
            
            // Draw Title using the big decal if available, else text
            // (Assumes you might have a logo, otherwise draw text)
            DrawString(ScreenWidth() / 2 - 80, 50, "OPERATION STARFALL", olc::CYAN, 2);
            DrawString(ScreenWidth() / 2 - 60, 80, "DEFEND DHAKA", olc::YELLOW, 1);

            // Draw Options
            DrawString(ScreenWidth() / 2 - 50, 130, "PRESS ENTER", olc::WHITE, 1);
            
            // Difficulty Selector
            std::string diffText = "< " + getDifficultyName() + " >";
            // Color based on difficulty
            olc::Pixel diffColor = (difficulty == Difficulty::EASY) ? olc::GREEN : 
                                   (difficulty == Difficulty::NORMAL) ? olc::YELLOW : olc::RED;
            
            DrawString(ScreenWidth() / 2 - 40, 150, diffText, diffColor, 1);
            
            DrawString(ScreenWidth() / 2 - 90, 180, "ARROWS: Select Difficulty", olc::GREY, 1);
            DrawString(ScreenWidth() / 2 - 60, 195, "Q: Quit Game", olc::GREY, 1);

            // Difficulty Selection Input
            if (GetKey(olc::Key::LEFT).bPressed) {
                if (difficulty == Difficulty::NORMAL) difficulty = Difficulty::EASY;
                else if (difficulty == Difficulty::HARD) difficulty = Difficulty::NORMAL;
                playSound(sndMenuSelect, 1.0f);
            }
            if (GetKey(olc::Key::RIGHT).bPressed) {
                if (difficulty == Difficulty::EASY) difficulty = Difficulty::NORMAL;
                else if (difficulty == Difficulty::NORMAL) difficulty = Difficulty::HARD;
                playSound(sndMenuSelect, 1.0f);
            }

            if (GetKey(olc::Key::ENTER).bPressed || GetKey(olc::Key::SPACE).bPressed) {
                state = GameState::STORY;
                storyIndex = 0;
                resetStorySlideState();  // Reset timers ONCE on enter
                storyMusicPlaying = false;  // Allow music to start
                playSound(sndMenuSelect, 1.0f);
            }
            if (GetKey(olc::Key::Q).bPressed) {
                return false; 
            }
            break;
        }
        
        case GameState::STORY:
        {
            // Start music on first frame (if not already playing)
            if (!storyMusicPlaying) {
                startStoryMusic(StoryMood::NEUTRAL);
            }

            if (storyIndex < storyDecals.size()) {
                bool skipOrAdvance = renderStorySlide(
                    storyDecals[storyIndex], 
                    storyText[storyIndex], 
                    StoryMood::NEUTRAL, 
                    dt
                );

                if (skipOrAdvance) {
                    // Check if ESC skip (skip entire story)
                    if (storySkipTimer >= 1.5f) {
                        stopStoryMusic();
                        startLevel(1);
                        introTimer = 0.0f;
                        state = GameState::LEVEL_INTRO;
                    } else {
                        // Normal advance
                        storyIndex++;
                        resetStorySlideState();
                        playSound(sndMenuSelect, 1.0f);
                        if (storyIndex >= storyDecals.size()) {
                            stopStoryMusic();
                                startLevel(1);
                            introTimer = 0.0f;
                            state = GameState::LEVEL_INTRO;
                        }
                    }
                }
            }
            break;
        }

        case GameState::STORY_LEVEL2:
        {
            // Start music if not already playing
            if (!storyMusicPlaying) {
                startStoryMusic(StoryMood::NEUTRAL);
            }

            if (storyL2Index < storyL2Decals.size()) {
                bool skipOrAdvance = renderStorySlide(
                    storyL2Decals[storyL2Index], 
                    storyL2Text[storyL2Index], 
                    StoryMood::NEUTRAL, 
                    dt
                );

                if (skipOrAdvance) {
                    if (storySkipTimer >= 1.5f) {
                        stopStoryMusic();
                        startLevel(2);
                        introTimer = 0.0f;
                        state = GameState::LEVEL_INTRO;
                    } else {
                        storyL2Index++;
                        resetStorySlideState();
                        playSound(sndMenuSelect, 1.0f);
                        if (storyL2Index >= storyL2Decals.size()) {
                            stopStoryMusic();
                                startLevel(2);
                            introTimer = 0.0f;
                            state = GameState::LEVEL_INTRO;
                        }
                    }
                }
            }
            break;
        }

        case GameState::STORY_LEVEL3:
        {
            // Start music if not already playing
            if (!storyMusicPlaying) {
                startStoryMusic(StoryMood::NEUTRAL);
            }

            if (storyL3Index < storyL3Decals.size()) {
                bool skipOrAdvance = renderStorySlide(
                    storyL3Decals[storyL3Index], 
                    storyL3Text[storyL3Index], 
                    StoryMood::NEUTRAL, 
                    dt
                );

                if (skipOrAdvance) {
                    if (storySkipTimer >= 1.5f) {
                        stopStoryMusic();
                        startLevel(3);
                        introTimer = 0.0f;
                        state = GameState::LEVEL_INTRO;
                    } else {
                        storyL3Index++;
                        resetStorySlideState();
                        playSound(sndMenuSelect, 1.0f);
                        if (storyL3Index >= storyL3Decals.size()) {
                            stopStoryMusic();
                                startLevel(3);
                            introTimer = 0.0f;
                            state = GameState::LEVEL_INTRO;
                        }
                    }
                }
            }
            break;
        }

        case GameState::STORY_GAMEOVER:
        {
            // Start music if not already playing
            if (!storyMusicPlaying) {
                startStoryMusic(StoryMood::SAD);
            }

            if (storyGOIndex < storyGODecals.size()) {
                bool skipOrAdvance = renderStorySlide(
                    storyGODecals[storyGOIndex], 
                    storyGOText[storyGOIndex], 
                    StoryMood::SAD, 
                    dt
                );

                if (skipOrAdvance) {
                    if (storySkipTimer >= 1.5f) {
                        stopStoryMusic();
                        state = GameState::GAME_OVER;
                    } else {
                        storyGOIndex++;
                        resetStorySlideState();
                        playSound(sndMenuSelect, 1.0f);
                        if (storyGOIndex >= storyGODecals.size()) {
                            stopStoryMusic();
                                state = GameState::GAME_OVER;
                        }
                    }
                }
            }
            break;
        }

        case GameState::STORY_VICTORY:
        {
            // Start music if not already playing
            if (!storyMusicPlaying) {
                startStoryMusic(StoryMood::TRIUMPHANT);
            }

            if (storyVictoryIndex < storyVictoryDecals.size()) {
                bool skipOrAdvance = renderStorySlide(
                    storyVictoryDecals[storyVictoryIndex], 
                    storyVictoryText[storyVictoryIndex], 
                    StoryMood::TRIUMPHANT, 
                    dt
                );

                if (skipOrAdvance) {
                    if (storySkipTimer >= 1.5f) {
                        stopStoryMusic();
                        state = GameState::GAME_OVER;
                    } else {
                        storyVictoryIndex++;
                        resetStorySlideState();
                        playSound(sndMenuSelect, 1.0f);
                        if (storyVictoryIndex >= storyVictoryDecals.size()) {
                            stopStoryMusic();
                                state = GameState::GAME_OVER;
                        }
                    }
                }
            }
            break;
        }

        case GameState::LEVEL_INTRO:
        {
            introTimer += dt;
            bool visible = fmodf(introTimer * 4.0f, 2.0f) < 1.0f;

            std::string title;
            if (currentLevel == 1)
                title = "LEVEL 1: ASTEROID BELT";
            else if (currentLevel == 2)
                title = "LEVEL 2: FRONTIER ZONE";
            else if (currentLevel == 3)
                title = "LEVEL 3: ORBITAL SIEGE";

            if (visible) {
                int tw = int(title.size() * 8 * 2);
                int x = ScreenWidth() / 2 - tw / 2;
                int y = ScreenHeight() / 2 - 10;
                DrawString(x, y, title, olc::WHITE, 2);
            }

            DrawString(ScreenWidth() / 2 - 90, ScreenHeight() / 2 + 30,
                "Press ENTER to Begin", olc::YELLOW, 1);

            if (GetKey(olc::Key::ENTER).bPressed) {
                state = GameState::PLAYING;
            }
            break;
        }

        case GameState::PLAYING:
        {
            // Ensure no story music is playing during gameplay
            forceStopAllStoryMusic();
            
            // Check for pause
            if (GetKey(olc::Key::ESCAPE).bPressed || GetKey(olc::Key::P).bPressed) {
                state = GameState::PAUSED;
                break;
            }

            // 2. DRAW BACKGROUND (Lowest layer)
            bgOffset += 40.0f * dt;
            if (bgOffset >= sprBackground->height)
                bgOffset -= sprBackground->height;

            // Draw background decals
            SetDecalMode(olc::DecalMode::ADDITIVE);
            DrawDecal({ 0.0f, -bgOffset }, decBackground);
            DrawDecal({ 0.0f, -bgOffset + sprBackground->height }, decBackground);

            // CRITICAL FIX: Reset the Decal Mode for all other entities (Player, Bullets, Enemies)
            // This ensures they are drawn correctly, typically with transparency/ALPHA blending.
            SetDecalMode(olc::DecalMode::NORMAL);

            // 3. RUN GAME LOGIC/UPDATE/COLLISIONS
            updateCurrentLevel(dt);

            // Get screen shake offset
            olc::vf2d shakeOff = getShakeOffset();

            // 4. DRAW ENTITIES (Middle layers) - with screen shake applied
            for (auto& a : asteroids) a.Draw(this);
            for (auto& e : enemies) e.Draw(this);
            if (boss.alive && currentLevel == 3) boss.Draw(this);
            for (auto& eb : enemyBullets) eb.Draw(this);
            for (auto& b : bullets) b.Draw(this);
            
            // Draw power-ups
            for (auto& p : powerups) p.Draw(this);
            
            // Draw player with shield effect if active
            player.Draw(this);
            if (hasShield()) {
                // Draw shield bubble around player
                DrawCircle(player.pos + shakeOff, int(player.r + 8), olc::BLUE);
                DrawCircle(player.pos + shakeOff, int(player.r + 10), olc::CYAN);
            }

            for (auto& exp : explosions) {
                // Ensure additive blending for glowing explosions
                SetDecalMode(olc::DecalMode::ADDITIVE);

                olc::vf2d size = { exp.decal->sprite->width * exp.scale, exp.decal->sprite->height * exp.scale };

                // Draw decal centered on the entity's position + shake
                DrawDecal(
                    exp.pos - size / 2.0f + shakeOff,
                    exp.decal,
                    { exp.scale, exp.scale }
                );
            }

            // 5. DRAW HUD (Top layer)

            // Solid black background for main HUD (left side)
            FillRect(0, 0, 220, 140, olc::Pixel(0, 0, 0, 240));
            DrawRect(0, 0, 220, 140, olc::WHITE); // Border

            std::string lvlText;
            if (currentLevel == 1)
                lvlText = "LEVEL 1: ASTEROID BELT";
            else if (currentLevel == 2)
                lvlText = "LEVEL 2: FRONTIER ZONE";
            else if (currentLevel == 3)
                lvlText = "LEVEL 3: ORBITAL SIEGE";

            // Draw Level Text
            DrawString(8, 8, lvlText, olc::WHITE, 1.5f);
            DrawLine(8, 25, 212, 25, olc::Pixel(100, 100, 100));

            // Draw Stats
            DrawString(8, 32, "Score: " + std::to_string(score), olc::YELLOW, 1.5f);
            DrawString(8, 50, "Lives: " + std::to_string(player.lives), olc::GREEN, 1.5f);
            DrawString(8, 68, "Hits: " + std::to_string(hits), olc::RED, 1.5f);

            // Active Power-ups indicator
            int powerY = 86;
            if (hasDoubleShot()) {
                DrawString(8, powerY, "2X " + std::to_string(int(doubleShotTimer)) + "s", olc::YELLOW, 1);
                powerY += 12;
            }
            if (hasSpeedBoost()) {
                DrawString(8, powerY, "SPD " + std::to_string(int(speedBoostTimer)) + "s", olc::CYAN, 1);
                powerY += 12;
            }
            if (hasShield()) {
                DrawString(8, powerY, "SH " + std::to_string(int(shieldTimer)) + "s", olc::BLUE, 1);
                powerY += 12;
            }
            
            // Difficulty indicator (right side of HUD)
            DrawString(130, 32, getDifficultyName(), 
                (difficulty == Difficulty::EASY) ? olc::GREEN : 
                (difficulty == Difficulty::NORMAL) ? olc::YELLOW : olc::RED, 1);

            // Objective display
            if (currentLevel == 1) {
                int timeLeft = int(std::max(0.0f, GameConfig::LEVEL1_DURATION - levelTime));
                DrawString(8, 120, "TIME: " + std::to_string(timeLeft) + "s", olc::CYAN, 1.5f);
            }
            else if (currentLevel == 2) {
                DrawString(8, 120, "KILLS: " + std::to_string(enemiesKilled) + "/" + std::to_string(GameConfig::LEVEL2_KILL_TARGET), olc::CYAN, 1.5f);
            }
            else if (currentLevel == 3) {
                // --- Right HUD Panel (Boss HP) ---
                int barW = 200;
                int barH = 15; // Slightly thinner bar
                int barX = ScreenWidth() - barW - 15; // Move closer to right edge
                int barY = 25; // Move higher up

                float hpRatio = boss.alive ? float(boss.hp) / float(boss.maxHp) : 0.0f;
                int hpW = int(barW * hpRatio);

                // Background box for boss HP area (Condensed to height 60)
                FillRect(barX - 10, barY - 25, barW + 20, 60, olc::Pixel(0, 0, 0, 240));
                DrawRect(barX - 10, barY - 25, barW + 20, 60, olc::WHITE);

                // Label above bar
                // Adjusted Y-coordinate (-15) to sit closer to the bar
                DrawString(barX + 65, barY - 15, "BOSS HP", olc::WHITE, 1.0f); // Reduced text scale for max compactness

                // HP bar outline
                DrawRect(barX - 2, barY - 2, barW + 4, barH + 4, olc::WHITE);
                // Background
                FillRect(barX, barY, barW, barH, olc::VERY_DARK_RED);
                // Current HP
                if (hpW > 0) {
                    olc::Pixel hpColor = hpRatio > 0.5f ? olc::GREEN : (hpRatio > 0.25f ? olc::YELLOW : olc::RED);
                    FillRect(barX, barY, hpW, barH, hpColor);
                }

                // HP text below bar
                // Adjusted Y-coordinate (+18) to sit closer to the bar
                std::string hpText = std::to_string(boss.hp) + " / " + std::to_string(boss.maxHp);
                DrawString(barX + 55, barY + 18, hpText, olc::WHITE, 1.5f);
            }


            // 6. LEVEL COMPLETE CHECK
            if (player.lives > 0) {
                if (currentLevel == 1) {
                    if (levelTime >= GameConfig::LEVEL1_DURATION) {
                        playSound(sndLevelComplete, 1.0f);
                        // Go to Level 2 Story instead of directly to Level 2
                        state = GameState::STORY_LEVEL2;
                        storyL2Index = 0;
                        resetStorySlideState();
                        storyMusicPlaying = false;
                    }
                }
                else if (currentLevel == 2) {
                    if (enemiesKilled >= GameConfig::LEVEL2_KILL_TARGET) {
                        playSound(sndLevelComplete, 1.0f);
                        // Go to Level 3 Story instead of direct Level 3
                        state = GameState::STORY_LEVEL3;
                        storyL3Index = 0;
                        resetStorySlideState();
                        storyMusicPlaying = false;
                    }
                }
                else if (currentLevel == 3) {
                    if (!boss.alive) {
                        playSound(sndLevelComplete, 1.0f);  // Victory!
                        // Go to Victory Story instead of directly to Game Over
                        state = GameState::STORY_VICTORY;
                        storyVictoryIndex = 0;
                        resetStorySlideState();
                        storyMusicPlaying = false;
                        wins = true;
                        saveHighScore();
                    }
                }
            }

            break;
        }

        case GameState::PAUSED:
        {
            // Draw semi-transparent overlay
            FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::Pixel(0, 0, 0, 180));

            // Draw pause menu
            DrawString(ScreenWidth() / 2 - 60, ScreenHeight() / 2 - 60, "PAUSED", olc::WHITE, 3);
            DrawString(ScreenWidth() / 2 - 100, ScreenHeight() / 2 + 10,
                "Press ESC or P to Resume", olc::YELLOW, 1);
            DrawString(ScreenWidth() / 2 - 95, ScreenHeight() / 2 + 35,
                "Press Q to Quit to Menu", olc::GREY, 1);

            // Current score display
            DrawString(ScreenWidth() / 2 - 70, ScreenHeight() / 2 + 70,
                "Score: " + std::to_string(score), olc::CYAN, 1);

            if (GetKey(olc::Key::ESCAPE).bPressed || GetKey(olc::Key::P).bPressed) {
                state = GameState::PLAYING;
            }
            if (GetKey(olc::Key::Q).bPressed) {
                saveHighScore();
                state = GameState::MENU;
            }
            break;
        }

        case GameState::GAME_OVER:
        {
            // Save high score when game ends (win or lose)
            saveHighScore();
            
            std::string line1 = wins ? "MISSION COMPLETE!" : "MISSION FAILED";
            std::string line2 = wins ? "Earth is Saved!" : "Earth has Fallen";
            std::string line3 = "Final Score: " + std::to_string(score);
            std::string line4 = "High Score: " + std::to_string(highScore);
            std::string line5 = "Press ENTER for Menu";
            
            // Check for new high score
            bool isNewHighScore = (score >= highScore && score > 0);

            int x1 = ScreenWidth() / 2 - int(line1.size()) * 8;
            int x2 = ScreenWidth() / 2 - int(line2.size()) * 8;
            int x3 = ScreenWidth() / 2 - int(line3.size()) * 8;
            int x4 = ScreenWidth() / 2 - int(line4.size()) * 4;
            int x5 = ScreenWidth() / 2 - int(line5.size()) * 4;

            olc::Pixel color1 = wins ? olc::GREEN : olc::RED;
            olc::Pixel color2 = wins ? olc::YELLOW : olc::DARK_RED;

            DrawString(x1, ScreenHeight() / 2 - 60, line1, color1, 2);
            DrawString(x2, ScreenHeight() / 2 - 25, line2, color2, 1);
            DrawString(x3, ScreenHeight() / 2 + 5, line3, olc::WHITE, 1);
            
            // High score with special color if new record
            olc::Pixel hsColor = isNewHighScore ? olc::YELLOW : olc::MAGENTA;
            DrawString(x4, ScreenHeight() / 2 + 30, line4, hsColor, 1);
            if (isNewHighScore) {
                DrawString(ScreenWidth() / 2 - 60, ScreenHeight() / 2 + 50, "NEW RECORD!", olc::YELLOW, 1);
            }
            
            DrawString(x5, ScreenHeight() / 2 + 75, line5, olc::CYAN, 1);

            if (GetKey(olc::Key::ENTER).bPressed) {
                state = GameState::MENU;
            }
            break;
        }
        }

        return true;
    }
};

int main()
{
    SpaceShooter game;
    if (game.Construct(900, 600, 1, 1))
        game.Start();
    return 0;
}