# Weather Sprites: Developer & Asset Guide

This document describes the architecture, implementation, and resource optimization of the weather sprite system in the Frixos project.

## 🏗️ Architecture Overview

The weather sprite system is designed to provide smooth, animated visual feedback while respecting the extreme memory constraints of the ESP32-WROOM (no PSRAM).

### 1. Sprite Engine ([main/f-sprite.c](main/f-sprite.c))
The core engine manages:
- **Image Offsets**: Animates by shifting the `x_offset` of a single contiguous horizontal sprite sheet.
- **High-Precision Timers**: Uses `esp_timer` for frame-accurate playback.
- **Phase Control**: Supports `loop_range` to play specific segments of a sprite sheet (e.g., just the looping cycle).
- **Blend Modes**: Uses `LV_BLEND_MODE_ADDITIVE` for transparent-like effects without alpha channel overhead.

### 2. Transition State Machine ([main/f-display.c](main/f-display.c))
Sprites transition through four distinct phases:
- `WAIT`: Static display of Frame 0. Timer is **stopped** to save CPU.
- `ENTRY`: 5-frame transition (fade-in/fill) from silent to active.
- `CYCLE`: The core looping animation.
- `EXIT`: 5-frame transition (fade-out/drain) back to Frame 0.

## 🛠️ Configuration & Settings

Developers can fine-tune the behavior via NVS/API settings:
- **`p66` (sprite_wait_s)**: Time spent in `WAIT` phase. Set to `0` to disable transitions and play the full loop continuously.
- **`p67` (sprite_cycle_s)**: Requested duration for the `CYCLE` phase. 
    - *Note*: The engine rounds this value **UP** to the nearest full cycle to ensure the animation doesn't cut off mid-frame.
    - *Constraint*: If `p67` is 0 and transitions are off, the system forces at least one full cycle.

## 🎨 Asset Creation & Generation

Weather sprites are not stored as common image files (PNG/GIF) but are procedurally generated to optimize binary size and alignment.

### Generation Workflow
1. **Modify Logic**: Edit `tools/gen_weather_sprites.py` to change colors, shapes, or animation math.
2. **Run Generator**:
   ```bash
   python tools/gen_weather_sprites.py
   ```
3. **Output**: The tool generates `.c` files in `main/assets/` (e.g., `sprite_rain.c`) containing a `const uint8_t` array in RGB565 (Big Endian) format.
4. **Compile**: Rebuild the project to link the new C arrays.

### Asset Requirements
- **Dimensions**: Fixed at 32x32 pixels per frame.
- **Transitions**: Transition-capable sprites MUST have exactly 5 entry frames and 5 exit frames.
- **Header**: The asset must be wrapped in a `frixos_sprite_asset_t` struct defined in [main/include/f-sprite.h](main/include/f-sprite.h).

## 🔋 Resource Optimization (Key Technical Choices)

To maintain a responsive UI with only ~35KB of free heap, the following choices were made:

### 1. No Alpha Channel (RGB565)
Instead of using ARGB8888 (4 bytes per pixel), we use RGB565 (2 bytes). 
- **Benefit**: 50% reduction in RAM and Flash usage.
- **Blending**: Transparency is achieved using **Additive Blending** (`LV_BLEND_MODE_ADDITIVE`), which is computationally cheaper than Alpha blending.

### 2. Vertical Strip Buffering
By using a horizontal sprite sheet and double buffering (2 x 128x8 lines), we avoid allocating a large 128x128 frame buffer. LVGL pulls only the required 32x32 area during the rendering of the specific lines.

### 3. Idle Timer Management
Animation timers are active **only** during playback (`ENTRY`, `CYCLE`, `EXIT`). During the `WAIT` phase, the `esp_timer` is stopped. This reduces background interrupt overhead and aids system stability.

### 4. Direct NVS Access
Settings like `p66`/`p67` are accessed directly from `uint8_t` variables synchronized with NVS, avoiding frequent JSON parsing or heavy configuration lookups during the high-speed display loop.
