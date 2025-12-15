#pragma once
#include "olcPixelGameEngine.h"
#include "GameConfig.h"

struct Player {
	olc::vf2d pos;
	float speed = GameConfig::PLAYER_SPEED;
	float r = GameConfig::PLAYER_RADIUS;
	int lives = GameConfig::PLAYER_STARTING_LIVES;

	float invincibleTimer = 0.0f; // for flicker

	olc::Decal* decal = nullptr;

	void Reset(const olc::vf2d& startPos);
	void Update(olc::PixelGameEngine* pge, float dt);
	void Draw(olc::PixelGameEngine* pge);
};