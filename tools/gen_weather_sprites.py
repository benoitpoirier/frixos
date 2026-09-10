import argparse
import math
import random
import os
from PIL import Image # Ajouté pour la prévisualisation GIF

SPRITE_WIDTH = 32
SPRITE_HEIGHT = 32
DEBUG_SPRITE_BORDER = False

# Définition des phases d'animation
TRANSITION_FRAMES = 15
NOMINAL_FRAMES = 30
TOTAL_PARTICLE_FRAMES = TRANSITION_FRAMES * 2 + NOMINAL_FRAMES # 60 frames

# Couleurs du Soleil
SUN_EDGE = (232, 114, 22)
SUN_MID = (255, 184, 58)
SUN_CORE = (255, 248, 188)
SUN_RAY = (255, 190, 76)
SUN_RAY_HOT = (255, 220, 108)

def smoothstep(edge0, edge1, x):
    t = max(0.0, min(1.0, (x - edge0) / (edge1 - edge0)))
    return t * t * (3.0 - 2.0 * t)

def blend(c1, c2, t):
    t = max(0.0, min(1.0, t))
    return (
        int(c1[0] + (c2[0] - c1[0]) * t),
        int(c1[1] + (c2[1] - c1[1]) * t),
        int(c1[2] + (c2[2] - c1[2]) * t),
    )

def mix_colors(*colors):
    count = len(colors)
    return (
        sum(color[0] for color in colors) // count,
        sum(color[1] for color in colors) // count,
        sum(color[2] for color in colors) // count,
    )

class SpriteCanvas:
    def __init__(self, width, height, frames):
        self.width = width
        self.height = height
        self.frames = frames
        self.total_width = width * frames
        self.pixels = [0] * (self.total_width * height * 2)

    def set_pixel(self, frame, x, y, color):
        if 0 <= x < self.width and 0 <= y < self.height:
            abs_x = frame * self.width + x
            idx = (y * self.total_width + abs_x) * 2
            r, g, b = color
            dither = ((x * 7 + y * 13) % 4) - 1.5
            r = max(0, min(255, int(r + dither)))
            g = max(0, min(255, int(g + dither)))
            b = max(0, min(255, int(b + dither)))
            rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
            self.pixels[idx] = rgb565 & 0xFF
            self.pixels[idx + 1] = (rgb565 >> 8) & 0xFF

    def sample(self, frame, x, y):
        if not (0 <= x < self.width and 0 <= y < self.height):
            return (0, 0, 0)
        abs_x = frame * self.width + x
        idx = (y * self.total_width + abs_x) * 2
        rgb565 = self.pixels[idx] | (self.pixels[idx + 1] << 8)
        return (((rgb565 >> 11) & 0x1F) << 3, ((rgb565 >> 5) & 0x3F) << 2, (rgb565 & 0x1F) << 3)

    def blend_pixel(self, frame, x, y, color, amount):
        self.set_pixel(frame, x, y, blend(self.sample(frame, x, y), color, amount))

    def add_pixel(self, frame, x, y, color, strength=1.0):
        if 0 <= x < self.width and 0 <= y < self.height:
            scaled = (int(color[0] * strength), int(color[1] * strength), int(color[2] * strength))
            self.set_pixel(frame, x, y, blend(self.sample(frame, x, y), scaled, min(1.0, strength)))

    def to_bytes(self):
        return bytes(self.pixels)

def finish_sprite(canvas):
    return canvas.to_bytes(), canvas.frames

def shift_cloud_parts(parts, dy):
    return [(cx, cy + dy, rx, ry, light, mid, dark, outline) for cx, cy, rx, ry, light, mid, dark, outline in parts]

# --- DESSIN DES VOLUMES 3D ---

def draw_soft_glow(canvas, frame, cx, cy, radius_x, radius_y, color, strength):
    for y in range(int(cy - radius_y - 1), int(cy + radius_y + 2)):
        for x in range(int(cx - radius_x - 1), int(cx + radius_x + 2)):
            dx = (x - cx) / max(radius_x, 1)
            dy = (y - cy) / max(radius_y, 1)
            dist_sq = dx * dx + dy * dy
            if dist_sq <= 1.0:
                canvas.blend_pixel(frame, x, y, color, (1.0 - dist_sq) * strength)

def draw_tapered_ray(canvas, frame, cx, cy, angle, inner_radius, length, width, color):
    ux = math.cos(angle)
    uy = math.sin(angle)
    px = -uy
    py = ux
    for step in range(length):
        t = step / max(length - 1, 1)
        center_x = cx + ux * (inner_radius + step)
        center_y = cy + uy * (inner_radius + step)
        local_width = max(0.7, width * (1.0 - t * 0.72))
        for side in range(-math.ceil(local_width), math.ceil(local_width) + 1):
            sx = round(center_x + px * side)
            sy = round(center_y + py * side)
            edge = abs(side) / max(local_width, 1)
            canvas.blend_pixel(frame, sx, sy, color, max(0.0, 1.0 - edge) * (0.95 - t * 0.35))

def draw_cloud_shape(canvas, frame, parts, phase=0.0, light_boost=0.0):
    """
    Dessine un nuage très contrasté. 
    Reflet presque blanc sur le dessus des volutes supérieures.
    Gris très clair sur le dessus des volutes sombres.
    Bord des volutes légèrement plus clair.
    """
    for idx, (cx, cy, rx, ry, light, mid, dark, outline) in enumerate(parts):
        local_phase_x = phase * 1.3 + idx * 1.7
        local_phase_y = phase * 1.1 + idx * 2.3
        
        morph_factor = 1.0 if rx > 5.0 else 0.5
        animated_rx = rx + morph_factor * math.sin(local_phase_x)
        animated_ry = ry + morph_factor * math.cos(local_phase_y)
        
        for y in range(int(cy - animated_ry - 2), int(cy + animated_ry + 3)):
            for x in range(int(cx - animated_rx - 2), int(cx + animated_rx + 3)):
                dx = (x - cx) / max(animated_rx, 1)
                dy = (y - cy) / max(animated_ry, 1)
                dist_sq = dx * dx + dy * dy
                
                edge_alpha = 1.0 - smoothstep(0.85, 1.05, math.sqrt(dist_sq))
                if edge_alpha <= 0.0: continue
                
                nz = math.sqrt(max(0.0, 1.0 - dist_sq))
                light_intensity = max(0.0, dx * (-0.3) + dy * (-0.8) + nz * 0.5) ** 1.4
                light_intensity = min(1.0, light_intensity + light_boost)
                
                # Contraste vertical augmenté : le dessus (dy < 0) est BEAUCOUP plus clair
                top_highlight = max(0.0, -dy) * 1.8 
                vertical_t = max(0.0, dy) * 1.5 # Assombrit fortement le bas
                
                base = blend(dark, mid, light_intensity)
                base = blend(base, dark, vertical_t)
                
                # Applique le reflet blanc/gris très clair sur le dessus de manière plus agressive
                color = blend(base, light, top_highlight * 1.5 + light_boost * 0.8)
                
                shadow_pocket = max(0.0, 1.0 - ((dx - 0.08) ** 2 * 3.2 + (dy - 0.18) ** 2 * 4.8))
                color = blend(color, dark, shadow_pocket * 0.34)
                
                # Bord des voluptes légèrement plus clair pour faire ressortir le relief
                if 0.75 < dist_sq <= 1.0:
                    edge_t = smoothstep(0.75, 1.0, dist_sq)
                    edge_light = blend(color, blend(light, outline, 0.5), edge_t * 0.6)
                    canvas.blend_pixel(frame, x, y, edge_light, edge_alpha)
                else:
                    canvas.blend_pixel(frame, x, y, color, edge_alpha)

def draw_sun_orb(canvas, frame, cx, cy, radius, phase):
    glow_radius = radius + 3.8
    draw_soft_glow(canvas, frame, cx, cy, glow_radius, glow_radius, SUN_RAY, 0.16)
    for y in range(int(cy - glow_radius - 1), int(cy + glow_radius + 2)):
        for x in range(int(cx - glow_radius - 1), int(cx + glow_radius + 2)):
            dx = x - cx
            dy = y - cy
            dist = math.sqrt(dx * dx + dy * dy)
            if dist <= radius:
                radial = dist / max(radius, 1)
                base = blend(SUN_MID, SUN_EDGE, radial ** 0.88)
                core_glow = max(0.0, 1.0 - radial * 1.45)
                highlight_dx = (dx + radius * 0.38) / max(radius, 1)
                highlight_dy = (dy + radius * 0.42) / max(radius, 1)
                highlight = max(0.0, 1.0 - (highlight_dx * highlight_dx + highlight_dy * highlight_dy) * 1.35)
                terminator = max(0.0, min(1.0, (dx * 0.45 + dy * 0.25 + radius) / (2.0 * radius)))
                warmth = 0.5 + 0.5 * math.sin(phase + dx * 0.18 - dy * 0.10)
                lit = blend(base, SUN_CORE, highlight * 0.95)
                lit = blend((186, 84, 16), lit, terminator)
                lit = blend(lit, SUN_CORE, core_glow * 0.72)
                lit = blend(lit, (255, 214, 112), warmth * 0.07)
                canvas.set_pixel(frame, x, y, lit)
            elif dist <= glow_radius:
                halo_t = 1.0 - (dist - radius) / max(glow_radius - radius, 1)
                canvas.blend_pixel(frame, x, y, SUN_RAY, halo_t * 0.09)

def draw_rotating_sun(canvas, frame, cx, cy, core_radius, phase, pulse_strength=1.0, long_ray_length=6.6, short_ray_base=3.4, ray_scale=1.0, ray_alpha=1.0):
    effective_phase = phase + math.pi / 2.0
    pulse = 0.5 + 0.5 * math.sin(effective_phase)
    rotation = phase * 0.22
    
    draw_sun_orb(canvas, frame, cx, cy, core_radius + (pulse - 0.5) * 0.9 * pulse_strength, phase)
    
    long_width = 1.8 + (pulse * 0.6 * pulse_strength)
    for idx in range(8):
        angle = rotation + idx * (math.pi / 4.0)
        draw_tapered_ray(canvas, frame, cx, cy, angle, core_radius + 1.8, max(2, round(long_ray_length * ray_scale)), long_width, blend((0, 0, 0), SUN_RAY_HOT, ray_alpha))
    
    short_scale = (short_ray_base + pulse * 3.5 * pulse_strength) * ray_scale
    for idx in range(8):
        angle = rotation + (idx + 0.5) * (math.pi / 4.0)
        draw_tapered_ray(canvas, frame, cx, cy, angle, core_radius + 1.2, max(2, round(short_scale)), 1.2, blend((0, 0, 0), SUN_RAY, ray_alpha * 0.92))

# --- PARTICULES (PLUIE, NEIGE) ---

class Particle:
    def __init__(self, p_type, idx, total, speed):
        self.type = p_type
        self.spawn_frame = int((idx / total) * TRANSITION_FRAMES)
        self.phase = (idx * 0.37) % 1.0 
        self.speed = speed
        if idx % 2 == 0:
            self.x_start = random.uniform(3.0, 14.0)
        else:
            self.x_start = random.uniform(17.0, 28.0)
        self.y_start = 14.0
        self.shape_id = random.randint(0, 4)

def update_particle_state(F, particle):
    y_start = particle.y_start
    D = 18.0 
    
    if F < TRANSITION_FRAMES:
        if F < particle.spawn_frame:
            return None
        t = (F - particle.spawn_frame) / max(1, (TRANSITION_FRAMES - particle.spawn_frame))
        y = y_start + particle.phase * D * t
        return (particle.x_start, y, 1.0)
        
    elif F < TRANSITION_FRAMES + NOMINAL_FRAMES:
        t_nom = ((F - TRANSITION_FRAMES) / NOMINAL_FRAMES * particle.speed + particle.phase) % 1.0
        y = y_start + t_nom * D
        opacity = 1.0
        if y > 27: opacity = max(0.0, 1.0 - (y - 27) / 5.0)
        return (particle.x_start, y, opacity)
        
    else:
        t_nom_end = (particle.speed + particle.phase) % 1.0
        y_end_start = y_start + t_nom_end * D
        t_end = (F - (TRANSITION_FRAMES + NOMINAL_FRAMES)) / NOMINAL_FRAMES * particle.speed
        y = y_end_start + t_end * D
        
        if y >= 32: return None 
        opacity = 1.0
        if y > 27: opacity = max(0.0, 1.0 - (y - 27) / 5.0)
        return (particle.x_start, y, opacity)

def draw_rain_drop(canvas, frame, x, y, opacity, diagonal=True):
    x, y = int(round(x)), int(round(y))
    if diagonal:
        x_offset = int((y - 14) * 0.5) 
    else:
        x_offset = int((y - 14) * 0.1)
    px = x + x_offset
    
    color_core = (200, 230, 255)
    color_trail = (100, 150, 220)
    
    canvas.blend_pixel(frame, px, y, color_core, opacity)
    canvas.blend_pixel(frame, px - 1, y - 1, color_trail, opacity * 0.8)
    canvas.blend_pixel(frame, px - 1, y, color_trail, opacity * 0.6)

def draw_snow_flake(canvas, frame, x, y, speed, shape_id, opacity):
    x, y = int(round(x)), int(round(y))
    if speed == 1:
        canvas.blend_pixel(frame, x, y, (255, 255, 255), opacity)
    else:
        center = (255, 255, 255)
        tip = (180, 200, 255)
        if shape_id % 2 == 0: 
            offsets = [(0,0), (-1,0), (1,0), (0,-1), (0,1)]
        else: 
            offsets = [(0,0), (-1,-1), (1,1), (-1,1), (1,-1)]
            
        for dx, dy in offsets:
            color = center if (dx, dy) == (0, 0) else tip
            canvas.blend_pixel(frame, x + dx, y + dy, color, opacity)

# --- GÉNÉRATEURS ---

def generate_clearsky(frames=NOMINAL_FRAMES):
    canvas = SpriteCanvas(SPRITE_WIDTH, SPRITE_HEIGHT, frames)
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_rotating_sun(canvas, frame, 16, 16, 7.0, phase)
    return finish_sprite(canvas)

def generate_fair(frames=NOMINAL_FRAMES):
    """Nuage blanc avec soleil très visible en haut à gauche"""
    canvas = SpriteCanvas(SPRITE_WIDTH, SPRITE_HEIGHT, frames)
    palette = (
        ((255, 255, 255), (245, 245, 250), (220, 220, 235), (180, 180, 200)),
        ((255, 255, 255), (240, 240, 245), (210, 210, 225), (170, 170, 190)),
        ((250, 250, 255), (230, 230, 240), (200, 200, 220), (160, 160, 180)),
        ((245, 245, 255), (225, 225, 235), (190, 190, 210), (150, 150, 170))
    )
    parts = shift_cloud_parts([
        (11.0, 15.0, 5.0, 3.5, *palette[0]),
        (17.0, 12.0, 6.0, 4.5, *palette[1]),
        (24.0, 15.0, 5.5, 4.0, *palette[2]),
        (18.0, 18.0, 8.0, 4.0, *palette[3])
    ], -3.0)
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_rotating_sun(canvas, frame, 9, 8, 6.0, phase) # Plus gros et visible
        draw_cloud_shape(canvas, frame, parts, phase)
    return finish_sprite(canvas)

def generate_partlycloudy(frames=NOMINAL_FRAMES):
    """Nuage gris clair avec soleil en arrière-plan haut à gauche"""
    canvas = SpriteCanvas(SPRITE_WIDTH, SPRITE_HEIGHT, frames)
    palette = (
        ((255, 255, 255), (215, 215, 230), (165, 165, 185), (105, 105, 125)),
        ((245, 245, 255), (205, 205, 220), (155, 155, 175), (95, 95, 115)),
        ((235, 235, 245), (195, 195, 210), (145, 145, 165), (85, 85, 105)),
        ((225, 225, 235), (185, 185, 200), (135, 135, 155), (75, 75, 95))
    )
    parts = shift_cloud_parts([
        (11.0, 15.0, 5.0, 3.5, *palette[0]),
        (17.0, 12.0, 6.0, 4.5, *palette[1]),
        (24.0, 15.0, 5.5, 4.0, *palette[2]),
        (18.0, 18.0, 8.0, 4.0, *palette[3])
    ], -3.0)
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_rotating_sun(canvas, frame, 9, 9, 5.0, phase) # Haut à gauche
        draw_cloud_shape(canvas, frame, parts, phase)
    return finish_sprite(canvas)

def generate_cloud():
    """Nuage gris clair + 5 particules lentes"""
    canvas = SpriteCanvas(SPRITE_WIDTH, SPRITE_HEIGHT, TOTAL_PARTICLE_FRAMES)
    palette = (
        ((255, 255, 255), (215, 215, 230), (165, 165, 185), (105, 105, 125)),
        ((245, 245, 255), (205, 205, 220), (155, 155, 175), (95, 95, 115)),
        ((235, 235, 245), (195, 195, 210), (145, 145, 165), (85, 85, 105)),
        ((225, 225, 235), (185, 185, 200), (135, 135, 155), (75, 75, 95))
    )
    parts = shift_cloud_parts([
        (9.0, 15.0, 5.0, 3.5, *palette[0]),
        (15.0, 12.0, 6.0, 4.5, *palette[1]),
        (22.0, 15.0, 5.5, 4.0, *palette[2]),
        (16.0, 18.0, 8.0, 4.0, *palette[3])
    ], -3.0)
    
    particles = [Particle("rain", i, 5, speed=1) for i in range(5)] 
    
    for F in range(TOTAL_PARTICLE_FRAMES):
        phase = (F / NOMINAL_FRAMES) * 2.0 * math.pi
        draw_cloud_shape(canvas, F, parts, phase)
        
        for p in particles:
            state = update_particle_state(F, p)
            if state:
                x, y, opacity = state
                draw_rain_drop(canvas, F, x, y, opacity, diagonal=False)
                
    return finish_sprite(canvas)

def generate_rain():
    """Nuage Gris + 15 particules rapides en diagonale"""
    canvas = SpriteCanvas(SPRITE_WIDTH, SPRITE_HEIGHT, TOTAL_PARTICLE_FRAMES)
    palette = (
        ((230, 230, 245), (180, 180, 200), (130, 130, 150), (80, 80, 100)),
        ((220, 220, 235), (170, 170, 190), (120, 120, 140), (70, 70, 90)),
        ((210, 210, 225), (160, 160, 180), (110, 110, 130), (60, 60, 80)),
        ((200, 200, 215), (150, 150, 170), (100, 100, 120), (50, 50, 70))
    )
    parts = shift_cloud_parts([
        (9.0, 13.0, 6.0, 4.0, *palette[0]),
        (16.0, 11.0, 7.0, 5.0, *palette[1]),
        (23.0, 13.0, 5.5, 4.0, *palette[2]),
        (16.0, 16.0, 9.0, 4.0, *palette[3])
    ], -2.0)
    
    particles = [Particle("rain", i, 15, speed=2) for i in range(15)] 
    
    for F in range(TOTAL_PARTICLE_FRAMES):
        phase = (F / NOMINAL_FRAMES) * 2.0 * math.pi
        draw_cloud_shape(canvas, F, parts, phase)
        
        for p in particles:
            state = update_particle_state(F, p)
            if state:
                x, y, opacity = state
                draw_rain_drop(canvas, F, x, y, opacity, diagonal=True)
                
    return finish_sprite(canvas)

def generate_storm():
    """Nuage gris + 20 particules diagonales + Orages multiples"""
    canvas = SpriteCanvas(SPRITE_WIDTH, SPRITE_HEIGHT, TOTAL_PARTICLE_FRAMES)
    palette = (
        ((210, 210, 225), (140, 140, 160), (80, 80, 100), (40, 40, 60)), 
        ((200, 200, 215), (130, 130, 150), (70, 70, 90), (35, 35, 55)),
        ((190, 190, 205), (120, 120, 140), (60, 60, 80), (30, 30, 50)),
        ((180, 180, 195), (110, 110, 130), (50, 50, 70), (25, 25, 45))
    )
    parts = shift_cloud_parts([
        (10.0, 13.0, 6.0, 4.5, *palette[0]),
        (16.0, 10.0, 7.5, 5.5, *palette[1]),
        (23.0, 13.0, 6.0, 4.5, *palette[2]),
        (16.0, 16.0, 9.0, 4.5, *palette[3])
    ], -2.0)
    
    particles = [Particle("rain", i, 20, speed=2) for i in range(20)] 
    
    for F in range(TOTAL_PARTICLE_FRAMES):
        phase = (F / NOMINAL_FRAMES) * 2.0 * math.pi
        light_boost = 0.0
        flash_color = None
        draw_thin_blue = False
        draw_thick_yellow = False
        
        nominal_F = F - TRANSITION_FRAMES
        
        if nominal_F == 5 or nominal_F == 6: 
            light_boost = 0.8
            flash_color = (100, 150, 255)
            
        elif 12 <= nominal_F <= 15: 
            light_boost = 1.0
            flash_color = (255, 255, 100)
            draw_thick_yellow = True
            
        elif nominal_F == 20 or nominal_F == 21: 
            light_boost = 0.4
            draw_thin_blue = True
            
        if flash_color:
            for y in range(SPRITE_HEIGHT):
                for x in range(SPRITE_WIDTH):
                    canvas.blend_pixel(F, x, y, flash_color, 0.15)
                    
        draw_cloud_shape(canvas, F, parts, phase, light_boost=light_boost)
        
        if draw_thin_blue:
            for y in range(15, 32):
                x = 16 + int(math.sin(y * 0.5) * 2)
                canvas.add_pixel(F, x, y, (150, 200, 255), 0.9)
                
        if draw_thick_yellow:
            for y in range(15, 32):
                x = 16 + int(math.sin(y * 0.8) * 3)
                for dx in range(-1, 2):
                    canvas.add_pixel(F, x+dx, y, (255, 255, 200), 1.0)
        
        for p in particles:
            state = update_particle_state(F, p)
            if state:
                x, y, opacity = state
                draw_rain_drop(canvas, F, x, y, opacity, diagonal=True)
                
    return finish_sprite(canvas)

def generate_snow():
    """Nuage Gris Blanc + Flocons lents (1px) et rapides (3x3)"""
    canvas = SpriteCanvas(SPRITE_WIDTH, SPRITE_HEIGHT, TOTAL_PARTICLE_FRAMES)
    palette = (
        ((255, 255, 255), (225, 225, 245), (185, 185, 215), (135, 135, 165)),
        ((250, 250, 255), (220, 220, 240), (180, 180, 210), (130, 130, 160)),
        ((245, 245, 255), (215, 215, 235), (175, 175, 205), (125, 125, 155)),
        ((240, 240, 250), (210, 210, 230), (170, 170, 200), (120, 120, 150))
    )
    parts = shift_cloud_parts([
        (9.0, 13.0, 6.0, 4.0, *palette[0]),
        (16.0, 11.0, 7.0, 5.0, *palette[1]),
        (23.0, 13.0, 5.5, 4.0, *palette[2]),
        (16.0, 16.0, 9.0, 4.0, *palette[3])
    ], -2.0)
    
    particles = []
    for i in range(12):
        speed = 1 if i < 6 else 2
        particles.append(Particle("snow", i, 12, speed=speed))
    
    for F in range(TOTAL_PARTICLE_FRAMES):
        phase = (F / NOMINAL_FRAMES) * 2.0 * math.pi
        draw_cloud_shape(canvas, F, parts, phase)
        
        for p in particles:
            state = update_particle_state(F, p)
            if state:
                x, y, opacity = state
                x += math.sin(y * 0.5 + p.phase * 10) * 1.5
                draw_snow_flake(canvas, F, x, y, p.speed, p.shape_id, opacity)
                
    return finish_sprite(canvas)


def generate_fog(frames=NOMINAL_FRAMES):
    """
    Brouillard : Filaments dont l'amplitude varie et s'inverse.
    Phase spatiale fixe. Fréquence 0.1 à 0.5.
    Boucle parfaite garantie par l'amplitude et le décalage.
    """
    canvas = SpriteCanvas(SPRITE_WIDTH, SPRITE_HEIGHT, frames)
    
    random.seed(777) # Graine pour un rendu organique reproductible
    
    lines = []
    # 4 filaments clairs
    for i in range(10):
        # Fréquence spatiale comprise entre 0.1 et 0.5
        f = random.uniform(0.25, 0.5)
        # Longueur d'onde correspondante
        L = 2 * math.pi / f
        # Le décalage total (vent) doit être un multiple entier de la longueur d'onde pour boucler parfaitement
        V = L * random.choice([0.5, 1])
        
        lines.append({
            "f": f,
            "V": V,
            "amp_max": random.uniform(0, 5),
            "base_y": random.uniform(4, 28),
            "width": random.uniform(2, 8),
            "phase_s": random.uniform(0, 2 * math.pi), # Phase spatiale fixe
            "K_amp": random.choice([1, 2]), # Fréquence temporelle de l'amplitude (cycles sur l'anim)
            "phi_A": random.uniform(0, 2 * math.pi), # Phase de l'amplitude
            "p_wind1": random.uniform(0, 2 * math.pi),
            "p_wind2": random.uniform(0, 2 * math.pi),
            "intensity": random.uniform(1, 5),
            "color": random.uniform(1, 5)
        })
        
       
    for frame in range(frames):
        t = frame / frames
        
        # Buffer 2D pour accumuler la densité
        densities = [[0.0 for _ in range(SPRITE_WIDTH)] for _ in range(SPRITE_HEIGHT)]
        
        for line in lines:
            # Décalage horizontal progressif (boucle parfaite car V = n * L)
            shift = t * line["V"]
            
            # Amplitude variable. Passe par 0 et devient négative, inversant la phase naturellement
            # Boucle parfaite car sin(0) = sin(2*pi) = sin(4*pi)
            current_amp = line["amp_max"] * math.sin(2.0 * math.pi * line["K_amp"] * t + line["phi_A"])
            
            for x in range(SPRITE_WIDTH):
                # 1. Forme de la ligne (phase fixe, seule l'amplitude change)
                y_center = line["base_y"] + current_amp * math.sin((x - shift) * line["f"] + line["phase_s"])
                
                # 2. Effet de vent (disparition sur ~5px)
                # Fréquences multiples de la fréquence de base pour garantir la boucle parfaite
                i1 = math.sin((x - shift) * line["f"] * 0.3 + line["p_wind1"])
                i2 = math.sin((x - shift) * line["f"] * 5.0 + line["p_wind2"])
                i2=1
                intensity = (i1 * i2 + line["intensity"]) * 0.25

                # Seuillage dur : le filament disparaît complètement par endroits
                if intensity < 0.35:
                    continue
                else:
                    # Réapparition progressive
                    intensity = (intensity - 0.35) / 0.65
                
                w = line["width"]
                y_min = max(0, int(y_center - w - 1))
                y_max = min(SPRITE_HEIGHT - 1, int(y_center + w + 1))
                
                for y in range(y_min, y_max + 1):
                    dist = abs(y - y_center)
                    if dist < w:
                        t_dist = dist / w
                        # Profil de densité : clair au centre, sombre aux bords
                        d = 0.8 - smoothstep(0.0, 1.0, t_dist)
                        d = d ** 3.0
                        d *= intensity
                        
                        densities[y][x] += d
        
        # Rendu final
        for y in range(SPRITE_HEIGHT):
            for x in range(SPRITE_WIDTH):
                # Masque Vignette strict de 4px sur tous les bords
                min_dist = min(x, y, 31 - x, 31 - y)
                mask = smoothstep(0.0, 4.0, min_dist)
                
                if mask <= 0.0:
                    canvas.set_pixel(frame, x, y, (0, 0, 0))
                    continue
                
                # La densité ne peut pas être négative (le noir absolu est 0)
                d = max(0.0, min(1.0, densities[y][x]))
                d *= mask
                
                if d > 0.02:
                    # Dégradés de gris froids bien visibles
                    gray = int(d * 100)
                    r = gray
                    g = gray + 3
                    b = gray + 10
                    canvas.set_pixel(frame, x, y, (r, g, b))
                else:
                    canvas.set_pixel(frame, x, y, (0, 0, 0))
                    
    return finish_sprite(canvas)


# --- EXPORT C ---

def save_to_c_file(filename, data, name, frames, width=SPRITE_WIDTH, height=SPRITE_HEIGHT, transition_frames=0):
    with open(filename, "w") as file:
        file.write("/*\n * AUTO-GENERATED FILE - DO NOT EDIT MANUALLY\n * Master Pixel Art Edition\n */\n\n")
        file.write('#include "f-sprite.h"\n\n')
        file.write(f'const uint8_t {name}_map[] = {{\n')
        for i in range(0, len(data), 16):
            chunk = data[i:i + 16]
            hex_vals = ", ".join(f"0x{byte:02x}" for byte in chunk)
            file.write(f'    {hex_vals},\n')
        file.write('};\n\n')
        file.write(f'const frixos_sprite_asset_t {name} = {{\n')
        file.write('  .image = {\n')
        file.write('    .header.cf = LV_COLOR_FORMAT_RGB565,\n')
        file.write(f'    .header.w = {width * frames},\n')
        file.write(f'    .header.h = {height},\n')
        file.write(f'    .data_size = {len(data)},\n')
        file.write(f'    .data = {name}_map,\n')
        file.write('  },\n')
        file.write('  .fps = FRIXOS_SPRITE_DEFAULT_FPS,\n')
        if transition_frames > 0:
            file.write(f'  .transition_frames = {transition_frames},\n')
        file.write('};\n')


# --- PRÉVISUALISATION GIF ---

def save_gif_preview(name, data, frames, width=SPRITE_WIDTH, height=SPRITE_HEIGHT, output_dir="artwork/weather", scale=8):
    os.makedirs(output_dir, exist_ok=True)
    
    pil_frames = []
    for f in range(frames):
        img = Image.new("RGB", (width, height))
        pixels = img.load()
        
        for y in range(height):
            for x in range(width):
                # Le canvas stocke les frames côte à côte dans un grand tableau
                abs_x = f * width + x
                idx = (y * (width * frames) + abs_x) * 2
                
                # Lecture du RGB565 depuis le buffer de bytes
                rgb565 = data[idx] | (data[idx + 1] << 8)
                # Conversion en RGB888 pour Pillow
                r = ((rgb565 >> 11) & 0x1F) << 3
                g = ((rgb565 >> 5) & 0x3F) << 2
                b = (rgb565 & 0x1F) << 3
                pixels[x, y] = (r, g, b)
        
        # Agrandissement sans filtre (NEAREST) pour garder l'aspect pixel art
        img_resized = img.resize((width * scale, height * scale), Image.NEAREST)
        pil_frames.append(img_resized)
        
    gif_path = os.path.join(output_dir, f"{name}.gif")
    # duration=66ms correspond à ~15fps
    pil_frames[0].save(gif_path, save_all=True, append_images=pil_frames[1:], duration=66, loop=0)
    print(f"Generated {gif_path}")


GENERATORS = {
    "sprite_clearsky": generate_clearsky,
    "sprite_fair": generate_fair,
    "sprite_partlycloudy": generate_partlycloudy,
    "sprite_cloud": generate_cloud,
    "sprite_rain": generate_rain,
    "sprite_storm": generate_storm,
    "sprite_snow": generate_snow,
    "sprite_fog": generate_fog,
}

def generate_assets(sprite_names=None):
    selected = sprite_names or list(GENERATORS.keys())
    for name in selected:
        result = GENERATORS[name]()
        if len(result) == 3:
            data, frames, tr_frames = result
        else:
            data, frames = result
            tr_frames = 0
            
        # Génération du fichier C
        save_to_c_file(f"main/assets/{name}.c", data, name, frames, transition_frames=tr_frames)
        print(f"Generated main/assets/{name}.c ({frames} frames)")
        
        # Génération de l'aperçu GIF
        try:
            save_gif_preview(name, data, frames)
        except Exception as e:
            print(f"Error generating GIF for {name}: {e}")

if __name__ == "__main__":
    generate_assets()
