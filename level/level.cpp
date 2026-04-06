// level.cpp
#include "level.h"
#include "raylib.h"
#include <cmath> 

static const Color TILE_A = { 60,  60,  80,  255 };
static const Color TILE_B = { 80,  80, 100,  255 };
static const Color WALL_C = { 100, 120, 140, 255 };

void Level_Draw()
{
    // ── Checkerboard floor ────────────────────
    const int   EXTENT   = 30;
    const float TILE     = 4.0f;

    for (int z = -EXTENT; z < EXTENT; z++)
    {
        for (int x = -EXTENT; x < EXTENT; x++)
        {
            Color c = ((x + z) & 1) ? TILE_A : TILE_B;
            DrawPlane(
                { x * TILE + TILE * 0.5f, 0.0f, z * TILE + TILE * 0.5f },
                { TILE, TILE }, c
            );
        }
    }

    // ── Arena boundary wall (invisible ceiling, just pillars) ─────
    const float ARENA   = 30.0f;
    const float PILLAR_H = 8.0f;
    const float PILLAR_W = 2.0f;
    const int   PILLARS  = 12;

    for (int i = 0; i < PILLARS; i++)
    {
        float angle = (2.0f * PI * i) / (float)PILLARS;
        float px    = cosf(angle) * ARENA;
        float pz    = sinf(angle) * ARENA;
        Vector3 pos = { px, PILLAR_H * 0.5f, pz };
        DrawCube(pos, PILLAR_W, PILLAR_H, PILLAR_W, WALL_C);
        DrawCubeWires(pos, PILLAR_W, PILLAR_H, PILLAR_W, DARKBLUE);
    }

    // ── Central cover boxes ───────────────────
    struct Box { float x, z, w, h, d; Color col; };
    Box boxes[] = {
        {  0,  0, 2, 2, 6, { 140, 100,  80, 255 } },
        { 10,  5, 3, 2, 3, {  80, 140, 100, 255 } },
        {-10, -5, 3, 2, 3, {  80, 100, 140, 255 } },
        {  5,-10, 3, 2, 3, { 140,  80, 100, 255 } },
        { -6, 10, 3, 2, 3, { 120, 120,  80, 255 } },
    };

    for (auto& b : boxes)
    {
        Vector3 pos = { b.x, b.h * 0.5f, b.z };
        DrawCube(pos, b.w, b.h, b.d, b.col);
        DrawCubeWires(pos, b.w, b.h, b.d, BLACK);
    }

    // ── Sky-box stand-in: just a big sphere ───
    DrawSphere({ 0, 0, 0 }, 120.0f, { 20, 20, 40, 255 });
}
