# FPS Shooter – raylib C++ Game

A simple wave-based first-person shooter built with **raylib** and **C++17**.

## Controls
| Key / Button | Action |
|---|---|
| W A S D | Move |
| Mouse | Aim |
| Left Mouse Button | Shoot |
| Space | Jump |
| ESC | Pause |

## Features
- Wave-based enemy spawning (gets harder each wave)
- Head-bob and camera look
- Health bar + score HUD
- Title screen & game-over screen
- Separate files for each system (player, bullet, enemy, level, game state)

---

## How to Build

### Option 1 – CMake (recommended, cross-platform)
```bash
mkdir build && cd build
cmake ..
cmake --build .
./fps_shooter          # Linux/Mac
fps_shooter.exe        # Windows
```
CMake will automatically download raylib 5.0 via FetchContent.

### Option 2 – Manual g++ (Linux / Mac, raylib already installed)
```bash
g++ main.cpp game.cpp player.cpp bullet.cpp enemy.cpp level.cpp \
    -lraylib -lm -lpthread -ldl -o fps_shooter
./fps_shooter
```

### Option 3 – MSVC (Windows, raylib already installed)
1. Open `x64 Native Tools Command Prompt for VS`
2. ```
   cl /std:c++17 main.cpp game.cpp player.cpp bullet.cpp enemy.cpp level.cpp ^
      /I"C:\raylib\include" /link "C:\raylib\lib\raylib.lib" opengl32.lib ^
      gdi32.lib winmm.lib /out:fps_shooter.exe
   ```
   Adjust the include/lib paths to where you installed raylib.

---

## File Structure
```
fps_shooter/
├── main.cpp        ← entry point, game loop, HUD, screens
├── game.h/cpp      ← shared game state (score, health, wave)
├── player.h/cpp    ← player movement, look, shooting
├── bullet.h/cpp    ← bullet spawn, movement, collision
├── enemy.h/cpp     ← enemy AI, wave spawning, damage
├── level.h/cpp     ← arena geometry (floor, pillars, cover)
└── CMakeLists.txt  ← build script
```
