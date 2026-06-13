#include "include/f-sprite.h"
#include "include/frixos.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

static const char *TAG = "F_SPRITE";

/**
 * @brief Timer callback that handles frame switching.
 * Runs in the context of the esp_timer task.
 */
static void sprite_timer_cb(void *arg) {
    frixos_sprite_t *sprite = (frixos_sprite_t *)arg;
    if (!sprite || !sprite->is_playing) return;

    // Calculate next frame
    sprite->current_frame = (sprite->current_frame + 1) % sprite->total_frames;

    // Update LVGL object - Requires lock!
    if (lvgl_port_lock(0)) {
        lv_image_set_offset_x(sprite->img_obj, -(sprite->current_frame * sprite->frame_width));
        lvgl_port_unlock();
    }
}

frixos_sprite_t* frixos_sprite_create(lv_obj_t *img_obj, uint16_t width, uint16_t height, uint16_t total_frames, uint8_t fps) {
    if (!img_obj || width == 0 || height == 0 || total_frames == 0 || fps == 0) {
        ESP_LOGE(TAG, "Invalid parameters for sprite creation");
        return NULL;
    }

    // Allocate memory for the structure (Internal RAM for speed)
    frixos_sprite_t *sprite = (frixos_sprite_t *)heap_caps_malloc(sizeof(frixos_sprite_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    if (!sprite) {
        ESP_LOGE(TAG, "Failed to allocate sprite structure");
        return NULL;
    }

    sprite->img_obj = img_obj;
    sprite->frame_width = width;
    sprite->frame_height = height;
    sprite->total_frames = total_frames;
    sprite->current_frame = 0;
    sprite->frame_duration_ms = 1000 / fps;
    sprite->is_playing = false;

    // Set initial size of the object to one frame
    lvgl_port_lock(0);
    lv_obj_set_size(img_obj, width, height);
    lv_image_set_offset_x(img_obj, 0);
    lv_obj_set_style_bg_opa(img_obj, 0, 0); // Ensure transparent background
    lvgl_port_unlock();

    // Create the high-precision timer
    const esp_timer_create_args_t timer_args = {
        .callback = &sprite_timer_cb,
        .arg = (void *)sprite,
        .name = "sprite_timer",
        .dispatch_method = ESP_TIMER_TASK // Run in timer task context
    };

    esp_err_t err = esp_timer_create(&timer_args, &sprite->timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create esp_timer: %s", esp_err_to_name(err));
        free(sprite);
        return NULL;
    }

    ESP_LOGI(TAG, "Sprite created: %dx%d, %d frames, %d FPS", width, height, total_frames, fps);
    return sprite;
}

esp_err_t frixos_sprite_play(frixos_sprite_t *sprite) {
    if (!sprite || sprite->is_playing) return ESP_OK;

    // Start timer (convert ms to us)
    esp_err_t err = esp_timer_start_periodic(sprite->timer, sprite->frame_duration_ms * 1000);
    if (err == ESP_OK) {
        sprite->is_playing = true;
        ESP_LOGD(TAG, "Animation started");
    }
    return err;
}

void frixos_sprite_set_blend_mode(frixos_sprite_t *sprite, lv_blend_mode_t mode) {
    if (!sprite || !sprite->img_obj) return;
    
    if (lvgl_port_lock(0)) {
        lv_obj_set_style_blend_mode(sprite->img_obj, mode, 0);
        lvgl_port_unlock();
    }
}

void frixos_sprite_set_opacity(frixos_sprite_t *sprite, lv_opa_t opa) {
    if (!sprite || !sprite->img_obj) return;
    
    if (lvgl_port_lock(0)) {
        lv_obj_set_style_image_opa(sprite->img_obj, opa, 0);
        lvgl_port_unlock();
    }
}

esp_err_t frixos_sprite_stop(frixos_sprite_t *sprite) {
    if (!sprite || !sprite->is_playing) return ESP_OK;

    esp_err_t err = esp_timer_stop(sprite->timer);
    if (err == ESP_OK) {
        sprite->is_playing = false;
        ESP_LOGD(TAG, "Animation stopped");
    }
    return err;
}

void frixos_sprite_set_frame(frixos_sprite_t *sprite, uint16_t frame) {
    if (!sprite || frame >= sprite->total_frames) return;

    sprite->current_frame = frame;
    if (lvgl_port_lock(0)) {
        lv_image_set_offset_x(sprite->img_obj, -(sprite->current_frame * sprite->frame_width));
        lvgl_port_unlock();
    }
}

void frixos_sprite_delete(frixos_sprite_t *sprite) {
    if (!sprite) return;
    
    frixos_sprite_stop(sprite);
    esp_timer_delete(sprite->timer);
    free(sprite);
}
