/*******************************************************************************************
*
*   raylib [core] example - 3d camera fps
*
*   Converted to C++ for MSVC
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define GRAVITY         32.0f
#define MAX_SPEED       20.0f
#define CROUCH_SPEED     5.0f
#define JUMP_FORCE      12.0f
#define MAX_ACCEL      150.0f
#define FRICTION         0.86f
#define AIR_DRAG         0.98f
#define CONTROL         15.0f
#define CROUCH_HEIGHT    0.0f
#define STAND_HEIGHT     1.0f
#define BOTTOM_HEIGHT    0.5f
#define NORMALIZE_INPUT  0

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
struct Body {
    Vector3 position = { 0 };
    Vector3 velocity = { 0 };
    Vector3 dir = { 0 };
    bool isGrounded = false;
};

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static Vector2 sensitivity = { 0.001f, 0.001f };
static Body    player = {};
static Vector2 lookRotation = { 0 };
static float   headTimer = 0.0f;
static float   walkLerp = 0.0f;
static float   headLerp = STAND_HEIGHT;
static Vector2 lean = { 0 };

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void DrawLevel(void);
static void UpdateCameraFPS(Camera* camera);
static void UpdateBody(Body* body, float rot, int side, int forward, bool jumpPressed, bool crouchHold);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera fps");

    Camera camera = { 0 };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.position = {
        player.position.x,
        player.position.y + (BOTTOM_HEIGHT + headLerp),
        player.position.z,
    };

    UpdateCameraFPS(&camera);

    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update
        Vector2 mouseDelta = GetMouseDelta();
        lookRotation.x -= mouseDelta.x * sensitivity.x;
        lookRotation.y += mouseDelta.y * sensitivity.y;

        int  sideway = (int)IsKeyDown(KEY_D) - (int)IsKeyDown(KEY_A);
        int  forward = (int)IsKeyDown(KEY_W) - (int)IsKeyDown(KEY_S);
        bool crouching = IsKeyDown(KEY_LEFT_CONTROL);

        UpdateBody(&player, lookRotation.x, sideway, forward, IsKeyPressed(KEY_SPACE), crouching);

        float delta = GetFrameTime();
        headLerp = Lerp(headLerp, (crouching ? CROUCH_HEIGHT : STAND_HEIGHT), 20.0f * delta);
        camera.position = {
            player.position.x,
            player.position.y + (BOTTOM_HEIGHT + headLerp),
            player.position.z,
        };

        if (player.isGrounded && ((forward != 0) || (sideway != 0)))
        {
            headTimer += delta * 3.0f;
            walkLerp = Lerp(walkLerp, 1.0f, 10.0f * delta);
            camera.fovy = Lerp(camera.fovy, 55.0f, 5.0f * delta);
        }
        else
        {
            walkLerp = Lerp(walkLerp, 0.0f, 10.0f * delta);
            camera.fovy = Lerp(camera.fovy, 60.0f, 5.0f * delta);
        }

        lean.x = Lerp(lean.x, (float)sideway * 0.02f, 10.0f * delta);
        lean.y = Lerp(lean.y, (float)forward * 0.015f, 10.0f * delta);

        UpdateCameraFPS(&camera);

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawLevel();
        EndMode3D();

        DrawRectangle(5, 5, 330, 75, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(5, 5, 330, 75, BLUE);
        DrawText("Camera controls:", 15, 15, 10, BLACK);
        DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
        DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
        DrawText(TextFormat("- Velocity Len: (%06.3f)",
            Vector2Length({ player.velocity.x, player.velocity.z })), 15, 60, 10, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateBody(Body* body, float rot, int side, int forward, bool jumpPressed, bool crouchHold)
{
    Vector2 input = { (float)side, (float)-forward };

#if defined(NORMALIZE_INPUT)
    if ((side != 0) && (forward != 0)) input = Vector2Normalize(input);
#endif

    float delta = GetFrameTime();

    if (!body->isGrounded) body->velocity.y -= GRAVITY * delta;

    if (body->isGrounded && jumpPressed)
    {
        body->velocity.y = JUMP_FORCE;
        body->isGrounded = false;
    }

    Vector3 front = { sinf(rot), 0.0f, cosf(rot) };
    Vector3 right = { cosf(-rot), 0.0f, sinf(-rot) };

    Vector3 desiredDir = {
        input.x * right.x + input.y * front.x,
        0.0f,
        input.x * right.z + input.y * front.z
    };
    body->dir = Vector3Lerp(body->dir, desiredDir, CONTROL * delta);

    float decel = (body->isGrounded ? FRICTION : AIR_DRAG);
    Vector3 hvel = { body->velocity.x * decel, 0.0f, body->velocity.z * decel };

    float hvelLength = Vector3Length(hvel);
    if (hvelLength < (MAX_SPEED * 0.01f)) hvel = { 0.0f, 0.0f, 0.0f };

    float speed = Vector3DotProduct(hvel, body->dir);
    float maxSpeed = (crouchHold ? CROUCH_SPEED : MAX_SPEED);
    float accel = Clamp(maxSpeed - speed, 0.0f, MAX_ACCEL * delta);

    hvel.x += body->dir.x * accel;
    hvel.z += body->dir.z * accel;

    body->velocity.x = hvel.x;
    body->velocity.z = hvel.z;

    body->position.x += body->velocity.x * delta;
    body->position.y += body->velocity.y * delta;
    body->position.z += body->velocity.z * delta;

    if (body->position.y <= 0.0f)
    {
        body->position.y = 0.0f;
        body->velocity.y = 0.0f;
        body->isGrounded = true;
    }
}

static void UpdateCameraFPS(Camera* camera)
{
    const Vector3 up = { 0.0f, 1.0f, 0.0f };
    const Vector3 targetOffset = { 0.0f, 0.0f, -1.0f };

    Vector3 yaw = Vector3RotateByAxisAngle(targetOffset, up, lookRotation.x);

    float maxAngleUp = Vector3Angle(up, yaw) - 0.001f;
    if (-(lookRotation.y) > maxAngleUp) lookRotation.y = -maxAngleUp;

    float maxAngleDown = -Vector3Angle(Vector3Negate(up), yaw) + 0.001f;
    if (-(lookRotation.y) < maxAngleDown) lookRotation.y = -maxAngleDown;

    Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));

    float pitchAngle = Clamp(-lookRotation.y - lean.y, -PI / 2 + 0.0001f, PI / 2 - 0.0001f);
    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, pitchAngle);

    float headSin = sinf(headTimer * PI);
    float headCos = cosf(headTimer * PI);
    const float stepRotation = 0.01f;

    camera->up = Vector3RotateByAxisAngle(up, pitch, headSin * stepRotation + lean.x);

    const float bobSide = 0.1f;
    const float bobUp = 0.15f;
    Vector3 bobbing = Vector3Scale(right, headSin * bobSide);
    bobbing.y = fabsf(headCos * bobUp);

    camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, walkLerp));
    camera->target = Vector3Add(camera->position, pitch);
}

static void DrawLevel(void)
{
    const int   floorExtent = 25;
    const float tileSize = 5.0f;
    const Color tileColor1 = { 150, 200, 200, 255 };

    for (int y = -floorExtent; y < floorExtent; y++)
    {
        for (int x = -floorExtent; x < floorExtent; x++)
        {
            if ((y & 1) && (x & 1))
                DrawPlane({ (float)x * tileSize, 0.0f, (float)y * tileSize }, { tileSize, tileSize }, tileColor1);
            else if (!(y & 1) && !(x & 1))
                DrawPlane({ (float)x * tileSize, 0.0f, (float)y * tileSize }, { tileSize, tileSize }, LIGHTGRAY);
        }
    }

    const Vector3 towerSize = { 16.0f, 32.0f, 16.0f };
    const Color   towerColor = { 150, 200, 200, 255 };

    Vector3 towerPos = { 16.0f, 16.0f, 16.0f };
    DrawCubeV(towerPos, towerSize, towerColor);
    DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

    towerPos.x *= -1;
    DrawCubeV(towerPos, towerSize, towerColor);
    DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

    towerPos.z *= -1;
    DrawCubeV(towerPos, towerSize, towerColor);
    DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

    towerPos.x *= -1;
    DrawCubeV(towerPos, towerSize, towerColor);
    DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

    DrawSphere({ 300.0f, 300.0f, 0.0f }, 100.0f, { 255, 0, 0, 255 });
}