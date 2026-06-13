# Frixos Project Instructions - Technical Constraints

This file contains critical technical constraints discovered during the migration to ESP-IDF v6.2.0 and LVGL v9.5.0 for the Frixos project. These must be respected to maintain system stability and UI functionality.

## 🛠 Critical Configuration (sdkconfig)

If `sdkconfig` is reset or modified, ensure these values are restored. Failure to do so will cause immediate **Stack Overflow** (Boot Loops) or **UI Failures** (Invisible digits).

### 1. Stack Sizes (Anti-Boot Loop)
LVGL 9 requires significantly larger stacks than LVGL 8.
- `CONFIG_ESP_MAIN_TASK_STACK_SIZE=8192` (Required for `app_main` calling LVGL functions).
- `CONFIG_LV_TIMER_TASK_STACK_SIZE=8192` (Required for the built-in LVGL timer task).
- `CONFIG_ESP_SYSTEM_EVENT_STACK_SIZE=4096` (Increased for network/system stability).

### 2. LVGL Graphics & Decoding
- `CONFIG_LV_USE_TJPGD=y` - **MANDATORY**. The clock digits are JPEG resources. Without this, the time will NOT appear.
- `CONFIG_LV_MEM_SIZE_KILOBYTES=64` - The maximum safe pool for internal DRAM.

## 🧠 Hardware & Memory Constraints (ESP32 WROOM)

- **No PSRAM**: The device only has ~320KB of internal DRAM.
- **Available Heap**: Approximately **~35KB** free after LVGL initialization.
- **Display**: ST7735 (128x128 resolution).
- **Color Format**: BGR (Swap enabled).
- **Buffering**: Double buffering is used (2 x 128x8 lines) to minimize RAM footprint while allowing smooth scrolling text.

## 📝 Coding Standards for Frixos

1. **Memory Safety**: Avoid large local arrays in functions. Prefer static or heap allocation if the buffer is needed long-term, but monitor the 35KB limit.
2. **Conditional UI**: Use `lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN)` and `lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN)` for the clock digits.
3. **Time Validation**: Always check `time_valid` before attempting to render or update the clock objects.
4. **Scrolling Text**: The scrolling text is the only element that can handle large string buffers due to its dynamic nature. Keep stock tokens within reasonable length.
5. **Language**: All comments, default labels, variable names, log messages, and inline strings in source code **must be written in English**. The only exception is the `language_*.json` translation files in `spiffs/`, which contain localized strings by design.
6. **Logging**: Use `ESP_LOGI`, `ESP_LOGW`, `ESP_LOGE` with the `TAG` defined in each module.
7. **Error Handling**: Use `ESP_RETURN_ON_ERROR` and `ESP_GOTO_ON_ERROR` macros for clean error propagation.
8. **Compilation**: NEVER run `idf.py build` (or any equivalent build task/command) at the end of a request unless specifically asked by the user.

## 🎞 Animation & Graphics (LVGL 9)

### Memory-Safe Animation
- **AVOID `lv_obj_set_style_transform_scale`**: On this target (ESP32-WROOM without PSRAM), scaling causes LVGL to allocate an intermediate ARGB8888 buffer (~5.4 KB to 11 KB). This often leads to immediate OOM/Watchdog restarts because the largest free heap block is typically < 5 KB.
- **Prefer Position Animation**: Use `lv_obj_set_pos` or `lv_obj_set_x/y` for transformations.
- **Fluidity & Resolution**: On a 128x128 screen, very slow movements (e.g., 1px every 2 seconds) will appear staggered. Use `roundf()` for coordinate calculation to minimize truncation artifacts.

### High-Performance Sprite Animation (`f-sprite.h`)
- **Library**: Use the custom `frixos_sprite_t` system for animated icons (weather, effects).
- **Zero-Copy**: Sprites MUST be stored as **Raw RGB565 C-Arrays** in Flash. Avoid JPEGs for high-FPS animations to prevent CPU bottlenecks and high RAM usage from the decoder.
- **Timing**: Animations use `esp_timer` (High Resolution Timer) instead of `lv_timer` to ensure a stable 15-30 FPS independently of the UI task's business logic.
- **Rendering Method**: Use `lv_image_set_offset_x` on a horizontal spritesheet. Ensure the `lv_obj_t` size matches exactly ONE frame.
- **Transparency & Blending**:
    - Use `lv_obj_set_style_bg_opa(obj, 0, 0)` on sprite objects.
    - For "Glow" or additive effects on black backgrounds, use `frixos_sprite_set_blend_mode(sprite, LV_BLEND_MODE_ADDITIVE)`.
    - Opacity can be controlled globally via `frixos_sprite_set_opacity`.
- **DMA Alignment**: The 40MHz SPI bus handles the pixel transfer via DMA. Ensure all UI updates in timer callbacks are wrapped in `lvgl_port_lock(0)`.

### UI Settings Toggling
- **Live Updates**: Use the pattern of `static last_setting` variables within a main loop or dedicated task (like `display_task`) to detect NVS global changes and apply them dynamically (e.g., creating/deleting timers or updating UI objects) without requiring a device reboot.
- **Conditional Visibility**: When a feature (like animation) is toggled off in the Web UI, its sub-parameters (speed, amplitude) should be hidden using the `hidden-field` CSS class in `index.html`/`index.js`.

## 🌐 Web UI (SPIFFS)

### Architecture
- Single-page application served from SPIFFS partition (`0x670000`, **1.5 MB max**).
- Files: `index.html`, `index.css`, `index.js`, 9 × `language_XX.json` (i18n), JPEG assets.
- **Files are NOT minified** — `spiffs_create_partition_image` packs the `./spiffs` folder as-is.
- JPEG assets dominate partition size; text files (HTML/CSS/JS/JSON) account for ~220 KB combined.

### Settings API
- Endpoint: `GET` / `POST /api/settings`
- Keys use short `pXX` names for HTTP efficiency (e.g. `p60`=static_ip, `p61`=static_gw, `p62`=static_nm, `p63`=static_dns).
- Group filters: `settings`, `advanced`, `integrations`, `theme`.
- Shared HTTP buffer pool: `get_shared_buffer()` in `f-membuffer.c`.
- Status endpoint: `GET /api/status` returns network info (`ip_address`, `ip_gw`, `ip_nm`, `ip_dns1`, `ip_dns2`).

### i18n Rules
- All user-facing strings use `data-i18n="key.path"` attributes; JS resolves them at runtime.
- Translation keys live in `spiffs/language_XX.json` (en, de, fr, it, pt, sv, da, pl, es).
- When adding a new UI label, always add the key to **all 9 language files**.
- Default fallback text in HTML must be in English.

### UI Layout Conventions
- Settings tab is structured into named `<div class="section">` blocks, each with a `<div class="section-header">` and `<div class="section-content">`.
- **Single Save button** per form, placed at the bottom in a standalone `<div class="section">`.
- Two-column layout (`.connection-layout`) uses `display: flex; align-items: stretch` with `flex: 1 1 0` on both columns so they share equal width and height.
- Responsive breakpoint at **820px**: columns stack vertically.
- Static IP panel is a collapsible section (`.static-ip-section`) toggled by a checkbox; hidden/shown via `style="display:none"` (browser-side HTML, not LVGL flags).

## 🔑 NVS Settings

- Namespace: `frixos`.
- All persistent settings are declared in `settings_table[]` in `main/main.c` with `SETTING_TYPE_STR` or numeric types.
- Extern declarations for cross-module access live in `main/include/frixos.h`.
- Static IP variables: `eeprom_static_ip[16]`, `eeprom_static_gw[16]`, `eeprom_static_nm[16]`, `eeprom_static_dns[40]` (keys p60–p63).
- Static IP applied in `connect_to_wifi()` (`f-provisioning.c`): `esp_netif_dhcpc_stop()` → `esp_netif_set_ip_info()` before `esp_wifi_connect()`. Falls back to DHCP on error.
- DNS field supports two comma-separated values (DNS1,DNS2) stored in a single NVS key.

## 🔗 Project Context
- **Website**: [buyfrixos.com](https://buyfrixos.com)
- **Support**: [buyfrixos.com/help-center/](https://buyfrixos.com/help-center/)
- **Target OS**: ESP-IDF 6.x (Ubuntu 24.04 Environment).
