# Operation Starfall 🚀

A story-driven 2D space shooter game built with C++ and the olcPixelGameEngine.

![Game Preview](assets/sprites/player_ship.png)

---

## 📥 Download & Install

### Option 1: Download ZIP (Easiest)
1. Click the green **"Code"** button above
2. Select **"Download ZIP"**
3. Extract the ZIP file to your desired location
4. Open `Operation_Starfall_2DGame.sln` in Visual Studio

### Option 2: Clone with Git
```bash
git clone https://github.com/Shovon021/Computer-Graphics.git
```

---

## 🛠️ Requirements

### Software
| Requirement | Details |
|-------------|---------|
| **IDE** | Visual Studio 2022 or 2019 |
| **Workload** | "Desktop Development with C++" (install via Visual Studio Installer) |
| **OS** | Windows 10 or Windows 11 |
| **C++ Standard** | C++17 or higher |

### Installing Visual Studio Workload
1. Open **Visual Studio Installer**
2. Click **Modify** on your Visual Studio installation
3. Check ✅ **"Desktop development with C++"**
4. Click **Modify** to install

---

## 🎮 Features

- **3 Levels**: Survival, enemy elimination, and boss fight
- **Cinematic Story**: 18 story slides with music and typewriter text effects
- **Power-Up System**: Double shot, speed boost, shield, extra life
- **3 Difficulty Levels**: Easy, Normal, Hard
- **Audio**: Sound effects and story music
- **High Score**: Persistent high score tracking

---

## 🎯 Controls

| Key | Action |
|-----|--------|
| **WASD / Arrow Keys** | Move player |
| **SPACE** | Fire bullets |
| **ENTER** | Start game / Advance story |
| **P** | Pause game |
| **ESC** (hold 1.5s) | Skip story |
| **Left/Right** | Change difficulty (menu) |

---

## 🚀 Building & Running

1. Open `Operation_Starfall_2DGame.sln` in Visual Studio
2. Select **Debug | x64** configuration from the toolbar
3. Press **Ctrl + Shift + B** to build
4. Press **F5** to run the game

> **Note**: Make sure the `assets/` folder is in the same directory as the executable.

---

## 📁 Project Structure

```
Operation_Starfall_2DGame/
├── Operation_Starfall_2DGame.cpp   # Main game logic (1800+ lines)
├── Operation_Starfall_2DGame.sln   # Visual Studio solution
├── olcPixelGameEngine.h            # Game engine
├── olcPGEX_MiniAudio.h             # Audio extension
├── miniaudio.h                     # Audio backend
├── src/                            # Game entities
│   ├── GameConfig.h                # Game constants
│   ├── player.h/.cpp               # Player ship
│   ├── asteroid.h/.cpp             # Asteroids
│   ├── enemy.h                     # Enemy ships
│   ├── enemy_boss.h                # Boss
│   └── bullet.h / enemy_bullet.h   # Projectiles
├── assets/
│   ├── sprites/                    # 27 image files
│   └── audio/                      # 12+ sound files
└── docs/
    └── SETUP_GUIDE.md              # Detailed setup guide
```

---

## 📖 Game Progression

1. **Level 1**: Survive for 25 seconds (asteroid field)
2. **Level 2**: Defeat 15 enemy ships
3. **Level 3**: Epic boss battle (200 HP)
4. **Victory!** 🎉

---

## ⚠️ Troubleshooting

| Issue | Solution |
|-------|----------|
| Build fails | Ensure C++ workload is installed and C++17 is selected |
| Missing sprites | Verify `assets/sprites/` folder exists |
| No audio | Check `assets/audio/` folder has .wav files |
| Game crashes | Run from Visual Studio, not by double-clicking .exe |

See [docs/SETUP_GUIDE.md](docs/SETUP_GUIDE.md) for detailed troubleshooting.

---

## 📝 License

This project uses the [olcPixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine) by OneLoneCoder (Javidx9).

---

Made with ❤️ using olcPixelGameEngine
