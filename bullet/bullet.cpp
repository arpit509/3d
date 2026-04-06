// bullet.cpp
#include "bullet.h"
#include "raymath.h"
#include <vector>

static std::vector<Bullet> sBullets;

void Bullets_Spawn(Vector3 origin, Vector3 dir)
{
    Bullet b;
    b.position  = origin;
    b.direction = Vector3Normalize(dir);
    b.life      = BULLET_LIFE;
    b.active    = true;
    sBullets.push_back(b);
}

void Bullets_Update(float dt)
{
    for (auto& b : sBullets)
    {
        if (!b.active) continue;

        b.position.x += b.direction.x * BULLET_SPEED * dt;
        b.position.y += b.direction.y * BULLET_SPEED * dt;
        b.position.z += b.direction.z * BULLET_SPEED * dt;

        b.life -= dt;
        if (b.life <= 0.0f) b.active = false;
    }

    // Compact the list periodically
    if (sBullets.size() > 200)
    {
        std::vector<Bullet> alive;
        alive.reserve(64);
        for (auto& b : sBullets)
            if (b.active) alive.push_back(b);
        sBullets = alive;
    }
}

void Bullets_Draw()
{
    for (const auto& b : sBullets)
        if (b.active)
            DrawSphere(b.position, BULLET_RADIUS, YELLOW);
}

void Bullets_Clear() { sBullets.clear(); }

std::vector<Bullet>& Bullets_Get() { return sBullets; }
