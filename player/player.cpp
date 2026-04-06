// player.cpp
#include "player.h"
#include "bullet.h"
#include "game.h"
#include "raymath.h"
#include <cmath>

Player gPlayer;

void Player::Reset()
{
    position    = { 0.0f, 0.0f, 0.0f };
    velocity    = { 0.0f, 0.0f, 0.0f };
    yaw         = 0.0f;
    pitch       = 0.0f;
    isGrounded  = false;
    shootTimer  = 0.0f;
    ammo        = 30;
    bobTimer    = 0.0f;
    bobLerp     = 0.0f;
}

void Player_Update(Player& p, float dt)
{
    // ── Mouse look ─────────────────────────────
    Vector2 mouse = GetMouseDelta();
    p.yaw   -= mouse.x * MOUSE_SENS;
    p.pitch -= mouse.y * MOUSE_SENS;
    p.pitch  = Clamp(p.pitch, -1.5f, 1.5f);

    // ── Movement input ─────────────────────────
    int sx = (int)IsKeyDown(KEY_D) - (int)IsKeyDown(KEY_A);
    int fwd = (int)IsKeyDown(KEY_W) - (int)IsKeyDown(KEY_S);

    Vector3 front = { sinf(p.yaw), 0.0f, cosf(p.yaw) };
    Vector3 right = { cosf(p.yaw), 0.0f, -sinf(p.yaw) };

    Vector3 wishDir = {
        right.x * sx + front.x * fwd,
        0.0f,
        right.z * sx + front.z * fwd
    };
    if (Vector3Length(wishDir) > 0.001f) wishDir = Vector3Normalize(wishDir);

    // Accelerate
    p.velocity.x += wishDir.x * PLAYER_SPEED * dt * 8.0f;
    p.velocity.z += wishDir.z * PLAYER_SPEED * dt * 8.0f;

    // Friction / air drag
    float drag = p.isGrounded ? PLAYER_FRICTION : AIR_DRAG;
    p.velocity.x *= drag;
    p.velocity.z *= drag;

    // Speed cap
    float hspeed = sqrtf(p.velocity.x * p.velocity.x + p.velocity.z * p.velocity.z);
    if (hspeed > MAX_SPEED)
    {
        float scale = MAX_SPEED / hspeed;
        p.velocity.x *= scale;
        p.velocity.z *= scale;
    }

    // Gravity
    if (!p.isGrounded) p.velocity.y -= GRAVITY * dt;

    // Jump
    if (p.isGrounded && IsKeyPressed(KEY_SPACE))
    {
        p.velocity.y  = JUMP_FORCE;
        p.isGrounded  = false;
    }

    // Integrate
    p.position.x += p.velocity.x * dt;
    p.position.y += p.velocity.y * dt;
    p.position.z += p.velocity.z * dt;

    // Floor clamp
    if (p.position.y <= FLOOR_Y)
    {
        p.position.y = FLOOR_Y;
        p.velocity.y = 0.0f;
        p.isGrounded  = true;
    }

    // ── Head bob ───────────────────────────────
    bool moving = (sx != 0 || fwd != 0) && p.isGrounded;
    if (moving)
    {
        p.bobTimer += dt * 8.0f;
        p.bobLerp = Lerp(p.bobLerp, 1.0f, dt * 8.0f);
    }
    else
    {
        p.bobLerp = Lerp(p.bobLerp, 0.0f, dt * 8.0f);
    }

    // ── Shooting ───────────────────────────────
    p.shootTimer -= dt;
    bool shooting = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    if (shooting && p.shootTimer <= 0.0f)
    {
        p.shootTimer = SHOOT_COOLDOWN;

        // Fire from camera eye, in look direction
        Vector3 eye = { p.position.x, p.position.y + PLAYER_HEIGHT, p.position.z };

        // Build look direction from yaw + pitch
        Vector3 lookDir = {
            cosf(p.pitch) * sinf(p.yaw),   // note: flip yaw for fire direction
            sinf(p.pitch),
            cosf(p.pitch) * cosf(p.yaw)
        };
        // Negate Z so forward = negative Z in raylib convention
        lookDir.x =  cosf(p.pitch) * sinf(p.yaw);
        lookDir.y =  sinf(p.pitch);
        lookDir.z = -cosf(p.pitch) * cosf(p.yaw);   // camera target goes -z

        Bullets_Spawn(eye, lookDir);
    }
}

Camera Player_GetCamera(const Player& p)
{
    float bobX = sinf(p.bobTimer * PI)        * 0.06f * p.bobLerp;
    float bobY = fabsf(cosf(p.bobTimer * PI)) * 0.08f * p.bobLerp;

    Vector3 eye = {
        p.position.x + bobX,
        p.position.y + PLAYER_HEIGHT + bobY,
        p.position.z
    };

    // Target: camera looks in -Z by default, rotated by yaw then pitch
    Vector3 lookDir = {
        cosf(p.pitch) * sinf(p.yaw),
       -sinf(p.pitch),                     // negative: pitch up = negative y delta
        cosf(p.pitch) * cosf(p.yaw)        // cos(yaw) gives +Z (behind us in raylib)
    };
    // Correct: target is eye + forward, where forward flips Z
    lookDir = {
         cosf(p.pitch) * sinf(p.yaw),
        -sinf(p.pitch),
        -cosf(p.pitch) * cosf(p.yaw)
    };

    Camera cam = {};
    cam.position   = eye;
    cam.target     = Vector3Add(eye, lookDir);
    cam.up         = { 0.0f, 1.0f, 0.0f };
    cam.fovy       = 70.0f;
    cam.projection = CAMERA_PERSPECTIVE;
    return cam;
}
