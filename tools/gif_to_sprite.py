import sys
import os
from PIL import Image

def gif_to_frixos_sprite(gif_path, out_file_path, asset_name):
    if not os.path.exists(gif_path):
        print(f"Erreur : Le fichier {gif_path} n'existe pas.")
        return

    img = Image.open(gif_path)
    frames = []
    
    try:
        while True:
            bg = Image.new("RGB", img.size, (0, 0, 0))
            frame_rgba = img.convert("RGBA")
            bg.paste(frame_rgba, (0, 0), frame_rgba) 
            frames.append(bg)
            img.seek(img.tell() + 1)
    except EOFError:
        pass

    num_frames = len(frames)
    if num_frames == 0:
        print("Aucune frame trouvée.")
        return

    width, height = frames[0].size
    total_width = width * num_frames
    
    print(f"Conversion de {num_frames} frames ({width}x{height}) vers {out_file_path}...")

    c_array = []
    for y in range(height):
        for f in range(num_frames):
            for x in range(width):
                r, g, b = frames[f].getpixel((x, y))
                rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
                
                c_array.append(rgb565 & 0xFF)
                c_array.append((rgb565 >> 8) & 0xFF)

    with open(out_file_path, "w") as f:
        f.write(f"/* Fichier genere depuis {gif_path} */\n")
        f.write(f"#include \"f-sprite.h\"\n\n")
        f.write(f"const uint8_t {asset_name}_map[] = {{\n")
        
        for i in range(0, len(c_array), 16):
            chunk = c_array[i:i+16]
            f.write("  " + ", ".join(f"0x{b:02x}" for b in chunk) + ",\n")

        f.write(f"}};\n\n")
        f.write(f"const frixos_sprite_asset_t {asset_name} = {{\n")
        f.write(f"  .image = {{\n")
        f.write(f"    .header.cf = LV_COLOR_FORMAT_RGB565,\n")
        f.write(f"    .header.w = {total_width},\n")
        f.write(f"    .header.h = {height},\n")
        f.write(f"    .data_size = {len(c_array)},\n")
        f.write(f"    .data = {asset_name}_map,\n")
        f.write(f"  }},\n")
        f.write(f"  .fps = FRIXOS_SPRITE_DEFAULT_FPS,\n")
        f.write(f"  .transition_frames = 0,\n")
        f.write(f"}};\n")
        
    print(f"Fichier exporte avec succes vers {out_file_path} ({len(c_array)} bytes) ! 🎉")

if __name__ == "__main__":
    gif_path = "artwork/moon.gif"
    out_path = "main/assets/sprite_clearsky_night.c"
    asset_name = "sprite_clearsky_night"
    
    gif_to_frixos_sprite(gif_path, out_path, asset_name)
