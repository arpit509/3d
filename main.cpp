// ─────────────────────────────────────────────────────────────────────────────
//  main.cpp  –  FPS Shooter  (raylib + C++)
//  Build:  g++ main.cpp game.cpp player.cpp bullet.cpp enemy.cpp level.cpp
//             -lraylib -lm -o fps_shooter
//  Or use the provided CMakeLists.txt
// ─────────────────────────────────────────────────────────────────────────────

#include "raylib.h"
#include "raymath.h"

#include "game.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "level.h"

// ── Forward declarations ──────────────────────────────────────────────────────
static void DrawHUD();
static void DrawTitleScreen();
static void DrawGameOverScreen();
static void StartWave(int wave);

// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    const int W = 1600 , H = 900;
    InitWindow(W, H, "FPS Shooter  |  WASD Move  |  Mouse Aim  |  LMB Shoot  |  Space Jump");
    SetTargetFPS(60);
    DisableCursor();

    // ── Init ──────────────────────────────────────────────────────────────────
    gPlayer.Reset();

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // ── ESC toggles pause while playing ──────────────────────────────────
        if (gGame.screen == SCREEN_PLAY && IsKeyPressed(KEY_ESCAPE))
        {
            gGame.paused = !gGame.paused;
            if (gGame.paused) EnableCursor();
            else              DisableCursor();
        }

        // ══ TITLE SCREEN ════════════════════════════════════════════════════
        if (gGame.screen == SCREEN_TITLE)
        {
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                gGame.Reset();
                gPlayer.Reset();
                Bullets_Clear();
                Enemies_Clear();
                StartWave(gGame.wave);
                DisableCursor();
            }

            BeginDrawing();
            ClearBackground({ 10, 10, 20, 255 });
            DrawTitleScreen();
            EndDrawing();
            continue;
        }

        // ══ GAME OVER SCREEN ════════════════════════════════════════════════
        if (gGame.screen == SCREEN_GAMEOVER)
        {
            EnableCursor();
            if (IsKeyPressed(KEY_ENTER))
            {
                gGame.screen = SCREEN_TITLE;
            }

            BeginDrawing();
            ClearBackground({ 10, 0, 0, 255 });
            DrawGameOverScreen();
            EndDrawing();
            continue;
        }

        // ══ GAMEPLAY ════════════════════════════════════════════════════════
        if (!gGame.paused)
        {
            Player_Update(gPlayer, dt);
            Bullets_Update(dt);
            Enemies_Update(dt, gPlayer.position);

            // Wave complete?
            if (Enemies_AliveCount() == 0)
            {
                gGame.wave++;
                StartWave(gGame.wave);
            }
        }

        Camera cam = Player_GetCamera(gPlayer);

        BeginDrawing();
        ClearBackground({ 20, 20, 40, 255 });

        BeginMode3D(cam);
            Level_Draw();
            Enemies_Draw();
            Bullets_Draw();
        EndMode3D();

        DrawHUD();

        if (gGame.paused)
        {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0,0,0,120 });
            const char* txt = "PAUSED  –  ESC to resume";
            DrawText(txt,
                GetScreenWidth()/2  - MeasureText(txt, 30)/2,
                GetScreenHeight()/2 - 15, 30, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
static void StartWave(int wave)
{
    Bullets_Clear();
    Enemies_SpawnWave(wave);
    gGame.enemiesLeft = Enemies_AliveCount();
    // Refill health a bit between waves (not full – adds tension)
    gGame.health = Clamp(gGame.health + 30, 0, 100);
}

// ─────────────────────────────────────────────────────────────────────────────
//  HUD
// ─────────────────────────────────────────────────────────────────────────────
static void DrawHUD()
{
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // ── Crosshair ─────────────────────────────
    int cx = sw / 2, cy = sh / 2;
    DrawLine(cx - 12, cy,      cx + 12, cy,      WHITE);
    DrawLine(cx,      cy - 12, cx,      cy + 12, WHITE);
    DrawCircleLines(cx, cy, 4, { 255,255,255,120 });

    // ── Health bar ────────────────────────────
    float hpFrac = gGame.health / 100.0f;
    int barW = 220, barH = 22;
    int bx = 20, by = sh - 50;
    DrawRectangle(bx, by, barW, barH, { 60,0,0,200 });
    DrawRectangle(bx, by, (int)(barW * hpFrac), barH,
        hpFrac > 0.5f ? GREEN : hpFrac > 0.25f ? ORANGE : RED);
    DrawRectangleLines(bx, by, barW, barH, WHITE);
    DrawText(TextFormat("HP  %d", gGame.health), bx + 6, by + 4, 14, WHITE);

    // ── Score ─────────────────────────────────
    DrawText(TextFormat("SCORE  %06d", gGame.score), sw - 220, 14, 22, YELLOW);

    // ── Wave ──────────────────────────────────
    DrawText(TextFormat("WAVE  %d", gGame.wave), sw/2 - 50, 14, 22, SKYBLUE);

    // ── Enemies left ──────────────────────────
    int alive = Enemies_AliveCount();
    DrawText(TextFormat("ENEMIES  %d", alive), sw/2 - 60, 42, 18, LIGHTGRAY);

    // ── Controls reminder (top-left, small) ───
    DrawText("WASD Move | Mouse Aim | LMB Shoot | Space Jump | ESC Pause",
             10, 10, 12, { 200,200,200,180 });
}

// ─────────────────────────────────────────────────────────────────────────────
//  Title & Game-Over screens
// ─────────────────────────────────────────────────────────────────────────────
static void DrawTitleScreen()
{
    int sw = GetScreenWidth(), sh = GetScreenHeight();

    // Decorative lines
    for (int i = 0; i < 12; i++)
    {
        float a = (float)i / 12.0f * PI * 2.0f;
        int x1 = sw/2, y1 = sh/2;
        int x2 = x1 + (int)(cosf(a) * 350);
        int y2 = y1 + (int)(sinf(a) * 350);
        DrawLine(x1, y1, x2, y2, { 255, 60, 60, 40 });
    }

    DrawText("FPS  SHOOTER",
        sw/2 - MeasureText("FPS  SHOOTER", 72)/2,
        sh/2 - 160, 72, { 255, 60, 60, 255 });

    DrawText("A raylib shooter  –  Survive the waves!",
        sw/2 - MeasureText("A raylib shooter  –  Survive the waves!", 22)/2,
        sh/2 - 60, 22, LIGHTGRAY);

    DrawText("WASD  Move  |  Mouse  Aim  |  LMB  Shoot  |  Space  Jump",
        sw/2 - MeasureText("WASD  Move  |  Mouse  Aim  |  LMB  Shoot  |  Space  Jump", 18)/2,
        sh/2,  18, { 200,200,200,220 });

    // Blink effect
    if ((int)(GetTime() * 2) % 2 == 0)
        DrawText("PRESS  ENTER  OR  CLICK  TO  START",
            sw/2 - MeasureText("PRESS  ENTER  OR  CLICK  TO  START", 26)/2,
            sh/2 + 80, 26, YELLOW);
}

static void DrawGameOverScreen()
{
    int sw = GetScreenWidth(), sh = GetScreenHeight();

    DrawText("GAME  OVER",
        sw/2 - MeasureText("GAME  OVER", 80)/2,
        sh/2 - 140, 80, { 255, 40, 40, 255 });

    DrawText(TextFormat("Final Score:  %d", gGame.score),
        sw/2 - MeasureText(TextFormat("Final Score:  %d", gGame.score), 32)/2,
        sh/2 - 20, 32, YELLOW);

    DrawText(TextFormat("Reached Wave:  %d", gGame.wave),
        sw/2 - MeasureText(TextFormat("Reached Wave:  %d", gGame.wave), 26)/2,
        sh/2 + 30, 26, LIGHTGRAY);

    if ((int)(GetTime() * 2) % 2 == 0)
        DrawText("PRESS  ENTER  TO  RETURN  TO  TITLE",
            sw/2 - MeasureText("PRESS  ENTER  TO  RETURN  TO  TITLE", 24)/2,
            sh/2 + 100, 24, WHITE);
}