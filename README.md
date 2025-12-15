# Operation Starfall 🚀

A story-driven 2D space shooter game built with C++ and the olcPixelGameEngine.

![Game Preview](assets/sprites/player_ship.png)

## 🎮 Features

- **3 Levels**: Survival, enemy elimination, and boss fight
- **Cinematic Story**: 18 story slides with music and typewriter text effects
- **Power-Up System**: Double shot, speed boost, shield, extra life
- **3 Difficulty Levels**: Easy, Normal, Hard
- **Audio**: Sound effects and story music
- **High Score**: Persistent high score tracking

## 🎯 Controls

| Key | Action |
|-----|--------|
| **WASD / Arrow Keys** | Move player |
| **SPACE** | Fire bullets |
| **ENTER** | Start game / Advance story |
| **P** | Pause game |
| **ESC** (hold 1.5s) | Skip story |
| **Left/Right** | Change difficulty (menu) |

## 🛠️ Requirements

- **Visual Studio 2022** (or 2019) with "Desktop Development with C++" workload
- **Windows 10/11**
- **C++17** or higher

## 🚀 Building

1. Open `Operation_Starfall_2DGame.sln` in Visual Studio
2. Select **Debug | x64** configuration
3. Press **Ctrl + Shift + B** to build
4. Press **F5** to run

## 📁 Project Structure

```
Operation_Starfall_2DGame/
├── Operation_Starfall_2DGame.cpp   # Main game logic
├── olcPixelGameEngine.h            # Game engine
├── olcPGEX_MiniAudio.h             # Audio extension
├── src/                            # Game entities
│   ├── GameConfig.h
│   ├── player.h/.cpp
│   ├── asteroid.h/.cpp
│   ├── enemy.h
│   ├── enemy_boss.h
│   └── bullet.h / enemy_bullet.h
├── assets/
│   ├── sprites/                    # Game graphics
│   └── audio/                      # Sound effects & music
└── docs/
    └── SETUP_GUIDE.md              # Detailed setup instructions
```

## 📖 Game Progression

1. **Level 1**: Survive for 25 seconds
2. **Level 2**: Defeat 15 enemy ships
3. **Level 3**: Boss battle (200 HP)
4. **Victory!** 🎉

## 📝 License

This project uses the olcPixelGameEngine by OneLoneCoder (Javidx9).
See [olcPixelGameEngine License](https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/LICENCE.md)

---
Made with ❤️ using olcPixelGameEngine
