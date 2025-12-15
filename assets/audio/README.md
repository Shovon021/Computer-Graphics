# Operation Starfall - Audio Files Required

The game now supports audio via `olcPGEX_MiniAudio`. 
Place WAV or MP3 files in `assets/audio/` with these names:

## Required Audio Files

| Filename | Event | Suggested Sound |
|----------|-------|-----------------|
| `shoot.wav` | Player fires | Short laser/pew sound |
| `explosion_small.wav` | Asteroid destroyed | Small explosion |
| `explosion_large.wav` | Enemy/boss destroyed | Large explosion |
| `powerup.wav` | Power-up collected | Positive chime/ding |
| `player_hit.wav` | Player takes damage | Hit/pain sound |
| `level_complete.wav` | Level cleared | Victory fanfare |
| `game_over.wav` | Player death | Defeat sound |
| `menu_select.wav` | Menu selection | Click/select sound |
| `boss_hit.wav` | Boss takes damage | Impact sound |

## Story Music (Optional)

| Filename | Usage | Suggested Style |
|----------|-------|-----------------|
| `story_epic.wav` | Prologue & Interludes | Epic orchestral, building tension |
| `story_sad.wav` | Mission Failed story | Somber, melancholy |
| `story_triumph.wav` | Victory story | Triumphant fanfare, celebration |

## Free Sound Resources

Get free game sounds from:
- [Freesound.org](https://freesound.org)
- [OpenGameArt.org](https://opengameart.org)
- [Mixkit.co](https://mixkit.co/free-sound-effects/game/)

## Notes

- WAV format recommended for low latency
- MP3 also supported
- Missing files won't crash the game (graceful fallback)
- Volume is controlled per-sound (0.0 to 1.0)
- Story music loops automatically during story sequences

