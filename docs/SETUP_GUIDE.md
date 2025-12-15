# Operation Starfall - Visual Studio Setup Guide

## 📋 Prerequisites

Before you begin, ensure you have:
- **Visual Studio 2022** (or 2019) with "Desktop Development with C++" workload
- **Windows 10/11**
- **Python 3.x** (optional, for generating audio files)

---

## 🚀 Step-by-Step Setup

### Step 1: Open the Project

1. Navigate to: `c:\Users\HP\Desktop\game\Operation_Starfall_2DGame\Operation_Starfall_2DGame\`
2. Double-click **`Operation_Starfall_2DGame.sln`** to open in Visual Studio

### Step 2: Select Build Configuration

1. In the toolbar, find the **Solution Configurations** dropdown
2. Select **`Debug`** (for testing) or **`Release`** (for best performance)
3. Select **`x64`** as the platform (recommended)

```
┌─────────────────────────────────────────────────┐
│ [Debug ▼]  [x64 ▼]  [▶ Local Windows Debugger] │
└─────────────────────────────────────────────────┘
```

### Step 3: Verify Project Properties

1. Right-click on **Operation_Starfall_2DGame** in Solution Explorer
2. Click **Properties**
3. Verify these settings:

#### C/C++ → General
| Setting | Value |
|---------|-------|
| Additional Include Directories | `$(ProjectDir)` |

#### C/C++ → Language
| Setting | Value |
|---------|-------|
| C++ Language Standard | `ISO C++17 Standard (/std:c++17)` |

#### Linker → System
| Setting | Value |
|---------|-------|
| SubSystem | `Console (/SUBSYSTEM:CONSOLE)` |

### Step 4: Build the Project

1. Press **Ctrl + Shift + B** to build
2. Or go to **Build → Build Solution**
3. Check the **Output** window for errors

Expected output:
```
========== Build: 1 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========
```

### Step 5: Run the Game

1. Press **F5** (with debugging) or **Ctrl + F5** (without debugging)
2. A console window and game window should appear

---

## 📁 Project Structure

```
Operation_Starfall_2DGame/
├── Operation_Starfall_2DGame.sln      # Solution file (open this!)
├── Operation_Starfall_2DGame.vcxproj  # Project file
├── Operation_Starfall_2DGame.cpp      # Main game code
├── olcPixelGameEngine.h               # Game engine
├── olcPGEX_MiniAudio.h                # Audio extension
├── miniaudio.h                        # Audio backend
├── src/
│   ├── GameConfig.h                   # Game constants
│   ├── player.h / player.cpp          # Player entity
│   ├── asteroid.h / asteroid.cpp      # Asteroid entity
│   ├── enemy.h                        # Enemy entity
│   ├── enemy_boss.h                   # Boss entity
│   ├── bullet.h                       # Player bullet
│   └── enemy_bullet.h                 # Enemy bullet
└── assets/
    ├── sprites/                       # 27 image files
    │   ├── player_ship.png
    │   ├── enemy_ship.png
    │   ├── boss_ship.png
    │   ├── story_1.jpg - story_5.png  # Prologue (5)
    │   ├── story_lvl2_1.jpg - 3.png   # Level 2 Interlude (3)
    │   ├── story_lvl3_1.png - 3.jpg   # Level 3 Interlude (3)
    │   ├── story_gameover_1-3.jpg     # Mission Failed (3)
    │   └── story_victory_1-4.jpg      # Victory (4)
    └── audio/                         # 12+ audio files
        ├── shoot.wav
        ├── explosion_small.wav
        ├── explosion_large.wav
        ├── powerup.wav
        ├── player_hit.wav
        ├── level_complete.wav
        ├── game_over.wav
        ├── menu_select.wav
        ├── boss_hit.wav
        ├── story_epic.wav             # Story music
        ├── story_sad.wav
        └── story_triumph.wav
```

---

## 🎮 Game Controls

| Key | Action |
|-----|--------|
| **Arrow Keys / WASD** | Move player |
| **SPACE** | Fire bullets |
| **ENTER** | Start game / Advance story |
| **ESC** (hold 1.5s) | Skip story |
| **P** | Pause game |
| **Q** | Quit (from menu) |
| **Left/Right** | Change difficulty (menu) |

---

## 🔊 Fixing Audio Issues

### If Story Music is Silent:

1. **Check Console Output** when game starts:
   ```
   Story Music Loading Status:
     - Epic: OK (ID: 9)
     - Sad: OK (ID: 10)
     - Triumph: OK (ID: 11)
   ```

2. If you see **FAILED**, regenerate the audio files:
   ```bash
   cd assets/audio
   python generate_story_music.py
   ```

3. Verify files exist:
   - `story_epic.wav` (~2.5 MB)
   - `story_sad.wav` (~2.5 MB)
   - `story_triumph.wav` (~2.5 MB)

---

## ⚠️ Common Issues & Fixes

### Issue: "Cannot open include file: 'olcPixelGameEngine.h'"
**Fix**: Ensure `Additional Include Directories` includes `$(ProjectDir)`

### Issue: Build fails with C++ errors
**Fix**: Set C++ Language Standard to **C++17** or higher

### Issue: Game crashes on startup
**Fix**: 
1. Run from Visual Studio (not by double-clicking .exe)
2. Working directory must contain `assets/` folder

### Issue: Sprites not showing
**Fix**: Check that all sprite files exist in `assets/sprites/`

### Issue: LNK2019: Unresolved external symbol
**Fix**: Ensure all `.cpp` files are included in the project:
- `Operation_Starfall_2DGame.cpp`
- `src/player.cpp`
- `src/asteroid.cpp`

---

## 🛠️ Rebuilding Assets

### Regenerate Sound Effects:
```bash
cd assets/audio
python generate_sounds.py
```

### Regenerate Story Music:
```bash
cd assets/audio
python generate_story_music.py
```

---

## ✅ Final Checklist

- [ ] Visual Studio 2022 installed with C++ workload
- [ ] Project opens without errors
- [ ] Configuration set to **Debug | x64**
- [ ] C++ Language Standard set to **C++17**
- [ ] All 27 sprite files present
- [ ] All 12 audio files present
- [ ] Build succeeds with 0 errors
- [ ] Game runs and shows menu
- [ ] Story sequence plays with images
- [ ] Music plays during stories
- [ ] Sound effects work during gameplay

---

## 🎬 Testing the Story System

1. **Prologue Test**: Start game → 5 story slides with Epic music
2. **Level 1**: Survive 25 seconds
3. **Interlude 1 Test**: 3 story slides
4. **Level 2**: Kill 15 enemies
5. **Interlude 2 Test**: 3 story slides
6. **Level 3**: Defeat the Boss
7. **Victory Test**: 4 victory slides with Triumphant music

**To test Game Over story**: Intentionally die → 3 sad slides
