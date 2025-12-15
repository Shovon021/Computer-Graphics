#pragma once
#include "olcPixelGameEngine.h"
#include "GameConfig.h"

struct Asteroid {
	olc::vf2d pos, vel;
	float r = GameConfig::ASTEROID_RADIUS_MIN;
	bool alive = true;

	olc::Decal* decal = nullptr;

	void Update(float dt, int screenH);
	void Draw(olc::PixelGameEngine* pge);

};