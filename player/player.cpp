// player.cpp
#include "player.h"
#include "bullet.h"
#include "game.h"
#include "raymath.h"
#include <cmath>

Player gPlayer;

void Player::Reset()
{
    position   = { 0.0f, 0.0f, 0.0f };
    velocity   = { 0.0f, 0.0f, 0.0f };
    yaw        = 0.0f;
    pitch      = 0.0f;
    isGrounded = false;
    shootTimer = 0.0f;
    ammo       = 30;
    bobTimer   = 0.0f;
    bobLerp    = 0.0f;
}

void Player_Update(Player& p, float dt)
{
    // ── Mouse look ────────────────────────────────────────────────────────────
    Vector2 mouse = GetMouseDelta();
    p.yaw   += mouse.x * MOUSE_SENS;   // right  = positive yaw
    p.pitch += mouse.y * MOUSE_SENS;   // down   = positive pitch
    p.pitch  = Clamp(p.pitch, -1.5f, 1.5f);

    // ── Movement input ────────────────────────────────────────────────────────
    int sx  = (int)IsKeyDown(KEY_D) - (int)IsKeyDown(KEY_A);
    int fwd = (int)IsKeyDown(KEY_W) - (int)IsKeyDown(KEY_S);

    // Forward and right vectors (horizontal plane only)
    Vector3 front = {  sinf(p.yaw), 0.0f,  cosf(p.yaw) };
    Vector3 right = { -cosf(p.yaw), 0.0f,  sinf(p.yaw) };  // D=right, A=left

    // Desired direction from input
    Vector3 wishDir = {
        right.x * (float)sx + front.x * (float)fwd,
        0.0f,
        right.z * (float)sx + front.z * (float)fwd
    };
    if (Vector3Length(wishDir) > 0.001f)
        wishDir = Vector3Normalize(wishDir);

    // ── Smooth horizontal movement (Lerp to target velocity) ─────────────────
    float targetX = wishDir.x * MAX_SPEED;
    float targetZ = wishDir.z * MAX_SPEED;

    // Higher rates = snappier, lower = more floaty
    bool  hasInput  = (sx != 0 || fwd != 0);
    float accelRate = p.isGrounded ? 12.0f : 4.0f;   // ramp up speed
    float decelRate = p.isGrounded ? 14.0f : 3.0f;   // slow down when no input
    float lerpRate  = hasInput ? accelRate : decelRate;

    p.velocity.x = Lerp(p.velocity.x, targetX, lerpRate * dt);
    p.velocity.z = Lerp(p.velocity.z, targetZ, lerpRate * dt);

    // ── Gravity ───────────────────────────────────────────────────────────────
    if (!p.isGrounded)
        p.velocity.y -= GRAVITY * dt;

    // ── Jump ──────────────────────────────────────────────────────────────────
    if (p.isGrounded && IsKeyPressed(KEY_SPACE))
    {
        p.velocity.y  = JUMP_FORCE;
        p.isGrounded  = false;
    }

    // ── Integrate position ────────────────────────────────────────────────────
    p.position.x += p.velocity.x * dt;
    p.position.y += p.velocity.y * dt;
    p.position.z += p.velocity.z * dt;

    // ── Floor collision ───────────────────────────────────────────────────────
    if (p.position.y <= FLOOR_Y)
    {
        p.position.y = FLOOR_Y;
        p.velocity.y = 0.0f;
        p.isGrounded = true;
    }

    // ── Head bob ──────────────────────────────────────────────────────────────
    bool moving = hasInput && p.isGrounded;
    if (moving)
    {
        p.bobTimer += dt * 8.0f;
        p.bobLerp = Lerp(p.bobLerp, 1.0f, dt * 8.0f);
    }
    else
    {
        p.bobLerp = Lerp(p.bobLerp, 0.0f, dt * 8.0f);
    }

    // ── Shooting ──────────────────────────────────────────────────────────────
    p.shootTimer -= dt;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && p.shootTimer <= 0.0f)
    {
        p.shootTimer = SHOOT_COOLDOWN;

        Vector3 eye = { p.position.x, p.position.y + PLAYER_HEIGHT, p.position.z };

        // Look direction from yaw + pitch
        Vector3 lookDir = {
             cosf(p.pitch) * sinf(p.yaw),
            -sinf(p.pitch),
             cosf(p.pitch) * cosf(p.yaw)
        };

        Bullets_Spawn(eye, lookDir);
    }
}

Camera Player_GetCamera(const Player& p)
{
    // Head bob offsets
    float bobX = sinf(p.bobTimer * PI)        * 0.06f * p.bobLerp;
    float bobY = fabsf(cosf(p.bobTimer * PI)) * 0.08f * p.bobLerp;

    Vector3 eye = {
        p.position.x + bobX,
        p.position.y + PLAYER_HEIGHT + bobY,
        p.position.z
    };

    // Camera look direction (matches bullet fire direction)
    Vector3 lookDir = {
         cosf(p.pitch) * sinf(p.yaw),
        -sinf(p.pitch),
         cosf(p.pitch) * cosf(p.yaw)
    };

    Camera cam    = {};
    cam.position  = eye;
    cam.target    = Vector3Add(eye, lookDir);
    cam.up        = { 0.0f, 1.0f, 0.0f };
    cam.fovy      = 70.0f;
    cam.projection = CAMERA_PERSPECTIVE;
    return cam;
}