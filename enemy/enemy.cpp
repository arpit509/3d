// enemy.cpp
#include "enemy.h"
#include "bullet.h"
#include "game.h"
#include "raymath.h"
#include <vector>
#include <cmath>

static std::vector<Enemy> sEnemies;

// Spawn positions ring the player at a distance
static Vector3 SpawnPos(int index, int total, float radius = 28.0f)
{
    float angle = (2.0f * PI * index) / (float)total;
    return { cosf(angle) * radius, 0.0f, sinf(angle) * radius };
}

void Enemies_SpawnWave(int wave)
{
    sEnemies.clear();
    int count = 3 + (wave - 1) * 2;   // 3, 5, 7, 9 … enemies per wave
    sEnemies.reserve(count);

    for (int i = 0; i < count; i++)
    {
        Enemy e;
        e.position    = SpawnPos(i, count);
        e.hp          = ENEMY_HP + (wave - 1) * 10;
        e.alive       = true;
        e.contactTimer = 0.0f;
        sEnemies.push_back(e);
    }
}

void Enemies_Update(float dt, Vector3 playerPos)
{
    auto& bullets = Bullets_Get();

    for (auto& e : sEnemies)
    {
        if (!e.alive) continue;

        // ── Move toward player ─────────────────────
        Vector3 toPlayer = Vector3Subtract(playerPos, e.position);
        toPlayer.y = 0;
        float dist = Vector3Length(toPlayer);

        if (dist > 0.1f)
        {
            Vector3 dir = Vector3Scale(Vector3Normalize(toPlayer), ENEMY_SPEED * dt);
            e.position = Vector3Add(e.position, dir);
        }

        // ── Contact damage ─────────────────────────
        e.contactTimer -= dt;
        if (dist < (ENEMY_RADIUS + 0.5f) && e.contactTimer <= 0.0f)
        {
            gGame.health -= ENEMY_DAMAGE;
            e.contactTimer = ENEMY_CONTACT_CD;
            if (gGame.health <= 0)
            {
                gGame.health = 0;
                gGame.screen = SCREEN_GAMEOVER;
            }
        }

        // ── Bullet collision ───────────────────────
        for (auto& b : bullets)
        {
            if (!b.active) continue;
            float d = Vector3Distance(b.position, e.position);
            if (d < (ENEMY_RADIUS + BULLET_RADIUS))
            {
                b.active = false;
                e.hp -= 25;
                if (e.hp <= 0)
                {
                    e.alive = false;
                    gGame.score += 100;
                    gGame.enemiesLeft--;
                    break;
                }
            }
        }
    }
}

void Enemies_Draw()
{
    for (const auto& e : sEnemies)
    {
        if (!e.alive) continue;

        // Body
        Vector3 bodyPos = { e.position.x, e.position.y + ENEMY_HEIGHT * 0.5f, e.position.z };
        DrawCube(bodyPos, ENEMY_RADIUS * 2, ENEMY_HEIGHT, ENEMY_RADIUS * 2, RED);
        DrawCubeWires(bodyPos, ENEMY_RADIUS * 2, ENEMY_HEIGHT, ENEMY_RADIUS * 2, DARKBROWN);

        // Head
        Vector3 headPos = { e.position.x, e.position.y + ENEMY_HEIGHT + 0.4f, e.position.z };
        DrawSphere(headPos, 0.35f, ORANGE);

        // HP bar (billboard-ish – draw above head in 3-D)
        // We draw a thin cube for the bar background / fill
        float hpFrac = Clamp((float)e.hp / (float)(ENEMY_HP), 0.0f, 1.0f);
        Vector3 barBase = { e.position.x - 0.5f, e.position.y + ENEMY_HEIGHT + 1.0f, e.position.z };
        DrawCube({ barBase.x + 0.5f, barBase.y, barBase.z }, 1.0f,  0.12f, 0.05f, DARKGRAY);
        DrawCube({ barBase.x + hpFrac * 0.5f, barBase.y, barBase.z - 0.01f },
                 hpFrac, 0.10f, 0.05f, GREEN);
    }
}

void Enemies_Clear() { sEnemies.clear(); }

int Enemies_AliveCount()
{
    int n = 0;
    for (const auto& e : sEnemies)
        if (e.alive) n++;
    return n;
}
