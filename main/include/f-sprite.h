#ifndef F_SPRITE_H
#define F_SPRITE_H

#include "lvgl.h"
#include "esp_timer.h"
#include "esp_err.h"

/**
 * @brief Sprite animation structure for Frixos.
 * Optimized for low RAM usage and DMA transfer.
 */
typedef struct {
    lv_obj_t *img_obj;          ///< LVGL Image object reference
    uint16_t frame_width;       ///< Width of a single frame in pixels
    uint16_t frame_height;      ///< Height of a single frame in pixels
    uint16_t total_frames;      ///< Total number of frames in the spritesheet
    uint16_t current_frame;     ///< Current active frame index
    uint16_t loop_start;        ///< First frame of playback loop range (default 0)
    uint16_t loop_end;          ///< Last frame of playback loop range inclusive (default total_frames-1)
    uint32_t frame_duration_ms; ///< Time between frames in milliseconds
    esp_timer_handle_t timer;   ///< Timer for high-precision frame updates
    bool is_playing;            ///< Animation status flag
} frixos_sprite_t;

#define FRIXOS_SPRITE_DEFAULT_FPS 10

/**
 * @brief Sprite asset descriptor with embedded default animation speed.
 * The image descriptor must stay first so the asset can still be passed
 * directly to LVGL APIs expecting an lv_image_dsc_t pointer.
 */
typedef struct {
    lv_image_dsc_t image;          ///< LVGL image descriptor for the spritesheet
    uint8_t fps;                   ///< Default transition speed between frames
    uint8_t transition_frames;     ///< Number of entry/exit transition frames (0 = no transitions)
} frixos_sprite_asset_t;

/**
 * @brief Initialize a sprite object and attach it to an LVGL image object.
 * 
 * @param img_obj Existing LVGL image object (must have its source set to a spritesheet)
 * @param width Width of one frame
 * @param height Height of one frame
 * @param total_frames Fallback number of frames when descriptor auto-detection is unavailable
 * @param fps Target frames per second
 * @return frixos_sprite_t* Pointer to the sprite structure or NULL on failure
 */
frixos_sprite_t* frixos_sprite_create(lv_obj_t *img_obj, uint16_t width, uint16_t height, uint16_t total_frames, uint8_t fps);

/**
 * @brief Start the animation loop.
 * @param sprite Pointer to the sprite structure
 * @return ESP_OK on success
 */
esp_err_t frixos_sprite_play(frixos_sprite_t *sprite);

/**
 * @brief Set the blending mode of the sprite (e.g., LV_BLEND_MODE_ADDITIVE).
 * Useful for "glitter", "glow" or black-background addition.
 * @param sprite Pointer to the sprite structure
 * @param mode LVGL blend mode
 */
void frixos_sprite_set_blend_mode(frixos_sprite_t *sprite, lv_blend_mode_t mode);

/**
 * @brief Set the global opacity of the sprite.
 * @param sprite Pointer to the sprite structure
 * @param opa Opacity (0 = transparent, 255 = opaque)
 */
void frixos_sprite_set_opacity(frixos_sprite_t *sprite, lv_opa_t opa);

/**
 * @brief Stop the animation loop.
 * @param sprite Pointer to the sprite structure
 * @return ESP_OK on success
 */
esp_err_t frixos_sprite_stop(frixos_sprite_t *sprite);

/**
 * @brief Set the current frame manually.
 * @param sprite Pointer to the sprite structure
 * @param frame Frame index (0 to total_frames - 1)
 */
void frixos_sprite_set_frame(frixos_sprite_t *sprite, uint16_t frame);

/**
 * @brief Delete the sprite structure and stop its timer.
 * @note This does NOT delete the lv_obj_t image object.
 * @param sprite Pointer to the sprite structure
 */
void frixos_sprite_delete(frixos_sprite_t *sprite);

/**
 * @brief Restrict the timer-driven loop to a sub-range of frames.
 * The timer wraps at loop_end back to loop_start instead of total_frames.
 * @param sprite Pointer to the sprite structure
 * @param start First frame of the loop range (clamped to valid range)
 * @param end  Last frame of the loop range inclusive (clamped to valid range)
 */
void frixos_sprite_set_loop_range(frixos_sprite_t *sprite, uint16_t start, uint16_t end);

#endif // F_SPRITE_H
