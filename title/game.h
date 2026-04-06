#pragma once

// ─────────────────────────────────────────────
//  game.h  –  global state & shared constants
// ─────────────────────────────────────────────

#include "raylib.h"

// ── Tuning constants ──────────────────────────
#define GRAVITY       28.0f
#define PLAYER_HEIGHT  1.7f   // eye height above floor
#define FLOOR_Y        0.0f

// ── Game state ────────────────────────────────
enum GameScreen { SCREEN_TITLE, SCREEN_PLAY, SCREEN_GAMEOVER };

struct GameState
{
    GameScreen  screen      = SCREEN_TITLE;
    int         score       = 0;
    int         health      = 100;
    int         wave        = 1;
    int         enemiesLeft = 0;   // enemies still alive this wave
    bool        paused      = false;

    void Reset()
    {
        score       = 0;
        health      = 100;
        wave        = 1;
        enemiesLeft = 0;
        paused      = false;
        screen      = SCREEN_PLAY;
    }
};

extern GameState gGame;   // defined in game.cpp
