# config.h is generated from .env by top-level CMakeLists.txt before components load

idf_component_register(SRCS 
    "main.c"
    "f-screen-layout.c"
    "f-wifi.c"
    "f-display.c"
    "f-time.c"
    "f-pwm.c"
    "ltr303.c"  
    "moon.c"
    "f-integrations.c"
    "f-settings.c"
    "f-provisioning.c"
    "f-ota.c"     
    "f-dns.c"    
    "f-sprite.c"
    "assets/frixos_8.c"
    "assets/frixos_9.c"
    "assets/frixos_10.c"
    "assets/frixos_11.c"
    "assets/frixos_12.c"
    "assets/frixos_14.c"
    "assets/frixos_16.c"
    "assets/font-pixelate_10.c"
    "f-membuffer.c"
    "f-json.c"
    "f-dexcom.c"
    "f-freestyle.c"
    "f-nightscout.c"
    "f-stocks.c"
    "assets/sprite_clearsky.c"
    "assets/sprite_clearsky_night.c"
    "assets/sprite_fair.c"
    "assets/sprite_fair_night.c"
    "assets/sprite_partlycloudy.c"
    "assets/sprite_partlycloudy_night.c"
    "assets/sprite_cloud.c"
    "assets/sprite_rain.c"
    "assets/sprite_storm.c"
    "assets/sprite_snow.c"
    "assets/sprite_fog.c"
    # "../managed_components/lvgl__lvgl/src/font/lv_font_montserrat_8.c"
    INCLUDE_DIRS "include"
    INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    REQUIRES spiffs
    REQUIRES esp_wifi
    REQUIRES nvs_flash
    REQUIRES esp_https_server
    REQUIRES esp_http_server
    REQUIRES esp_http_client
    REQUIRES app_update
    REQUIRES spi_flash
    REQUIRES esp_driver_gpio
    REQUIRES esp_driver_i2c
    REQUIRES esp_driver_spi
    REQUIRES esp_driver_ledc
    REQUIRES esp_timer
    REQUIRES esp_lcd
    REQUIRES esp_netif
    REQUIRES esp_event
    REQUIRES lwip
    REQUIRES mbedtls
    REQUIRES esp-tls
    REQUIRES vfs
    REQUIRES teriyakigod__esp_lcd_st7735)

    target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-jump-tables" "-fno-tree-switch-conversion")

