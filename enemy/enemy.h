#pragma once

// ─────────────────────────────────────────────
//  enemy.h
// ─────────────────────────────────────────────

#include "raylib.h"
#include <vector>

#define ENEMY_RADIUS      0.6f
#define ENEMY_HEIGHT      1.8f
#define ENEMY_SPEED       4.5f    // units / sec
#define ENEMY_DAMAGE     10       // HP on contact
#define ENEMY_CONTACT_CD  1.0f   // seconds between damage hits
#define ENEMY_HP         30

struct Enemy
{
    Vector3 position;
    int     hp;
    bool    alive;
    float   contactTimer;   // cooldown so player isn't hit every frame
};

// ── API ───────────────────────────────────────
void Enemies_SpawnWave(int wave);
void Enemies_Update(float dt, Vector3 playerPos);
void Enemies_Draw();
void Enemies_Clear();

int  Enemies_AliveCount();
