#pragma once

// ─────────────────────────────────────────────
//  bullet.h
// ─────────────────────────────────────────────

#include "raylib.h"
#include <vector>

#define BULLET_SPEED  60.0f
#define BULLET_LIFE    3.0f   // seconds before it expires
#define BULLET_RADIUS  0.15f

struct Bullet
{
    Vector3 position;
    Vector3 direction;   // normalised
    float   life;        // time remaining
    bool    active;
};

// ── API ───────────────────────────────────────
void   Bullets_Spawn(Vector3 origin, Vector3 dir);
void   Bullets_Update(float dt);
void   Bullets_Draw();
void   Bullets_Clear();

// Exposed so enemy.cpp can test collisions
std::vector<Bullet>& Bullets_Get();
