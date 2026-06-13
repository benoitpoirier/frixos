import math

def generate_rgb565_star(width=32, height=32, frames=10):
    """
    Generates a horizontal spritesheet of a rotating cross/star.
    Format: RGB565 (Big Endian for ESP32/LVGL)
    """
    total_width = width * frames
    pixels = [0] * (total_width * height * 2)

    for f in range(frames):
        angle = (f / frames) * math.pi / 2
        offset_x = f * width
        
        # Center of frame
        cx, cy = offset_x + width // 2, height // 2
        
        for i in range(-10, 11):
            # Draw two rotating bars
            for bar in [0, math.pi/2]:
                curr_angle = angle + bar
                px = int(cx + i * math.cos(curr_angle))
                py = int(cy + i * math.sin(curr_angle))
                
                if offset_x <= px < offset_x + width and 0 <= py < height:
                    idx = (py * total_width + px) * 2
                    # White color in RGB565: 0xFFFF
                    pixels[idx] = 0xFF
                    pixels[idx+1] = 0xFF
                    
    return bytes(pixels)

def save_to_c_file(filename, data, name="sprite_example"):
    with open(filename, "w") as f:
        f.write('#include "lvgl.h"\n\n')
        f.write(f'const uint8_t {name}_map[] = {{\n')
        for i in range(0, len(data), 16):
            chunk = data[i:i+16]
            hex_vals = ", ".join([f"0x{b:02x}" for b in chunk])
            f.write(f'    {hex_vals},\n')
        f.write('};\n\n')
        f.write(f'const lv_image_dsc_t {name} = {{\n')
        f.write(f'  .header.cf = LV_COLOR_FORMAT_RGB565,\n')
        f.write(f'  .header.w = 320,\n')
        f.write(f'  .header.h = 32,\n')
        f.write(f'  .data_size = {len(data)},\n')
        f.write(f'  .data = {name}_map,\n')
        f.write('};\n')

if __name__ == "__main__":
    data = generate_rgb565_star()
    save_to_c_file("main/sprite_example.c", data)
    print("Spritesheet generated: main/sprite_example.c")
