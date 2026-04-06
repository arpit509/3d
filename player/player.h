#pragma once

// ─────────────────────────────────────────────
//  player.h
// ─────────────────────────────────────────────

#include "raylib.h"

#define PLAYER_SPEED     10.0f
#define PLAYER_FRICTION   0.80f
#define AIR_DRAG          0.98f
#define JUMP_FORCE        10.0f
#define MAX_SPEED         14.0f
#define MOUSE_SENS         0.0015f
#define SHOOT_COOLDOWN     0.18f   // seconds between shots

struct Player
{
    Vector3 position;     // feet position
    Vector3 velocity;
    float   yaw;          // horizontal look angle (radians)
    float   pitch;        // vertical look angle (radians)
    bool    isGrounded;
    float   shootTimer;   // countdown to next allowed shot
    int     ammo;         // just for display – infinite refill per wave
    float   bobTimer;
    float   bobLerp;

    void Reset();
};

void   Player_Update(Player& p, float dt);
Camera Player_GetCamera(const Player& p);

extern Player gPlayer;
