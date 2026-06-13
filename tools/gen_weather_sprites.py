import argparse
import math


SPRITE_WIDTH = 32
SPRITE_HEIGHT = 32
DEBUG_SPRITE_BORDER = False
TRANSITION_FRAMES = 5  # entry/exit frames for rain and snow sprites

RAIN_CLOUD_LIGHT = (238, 244, 255)
RAIN_CLOUD_MID = (148, 168, 220)
RAIN_CLOUD_DARK = (62, 80, 132)
RAIN_CLOUD_BASE = (34, 49, 96)

SUN_EDGE = (232, 114, 22)
SUN_MID = (255, 184, 58)
SUN_CORE = (255, 248, 188)
SUN_RAY = (255, 190, 76)
SUN_RAY_HOT = (255, 220, 108)

MOON_RIM = (214, 222, 236)
MOON_FACE = (250, 252, 255)
MOON_HALO = (232, 238, 250)
MOON_CRATER = (222, 228, 240)
MOON_CRATER_SHADE = (164, 174, 192)


def blend(c1, c2, t):
    t = max(0.0, min(1.0, t))
    return (
        int(c1[0] + (c2[0] - c1[0]) * t),
        int(c1[1] + (c2[1] - c1[1]) * t),
        int(c1[2] + (c2[2] - c1[2]) * t),
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
            scaled = (
                int(color[0] * strength),
                int(color[1] * strength),
                int(color[2] * strength),
            )
            self.set_pixel(frame, x, y, blend(self.sample(frame, x, y), scaled, min(1.0, strength)))

    def to_bytes(self):
        return bytes(self.pixels)


def draw_debug_border(canvas, color=(255, 0, 0)):
    for frame in range(canvas.frames):
        for x in range(canvas.width):
            canvas.set_pixel(frame, x, 0, color)
            canvas.set_pixel(frame, x, canvas.height - 1, color)
        for y in range(canvas.height):
            canvas.set_pixel(frame, 0, y, color)
            canvas.set_pixel(frame, canvas.width - 1, y, color)


def finish_sprite(canvas):
    if DEBUG_SPRITE_BORDER:
        draw_debug_border(canvas)
    return canvas.to_bytes(), canvas.frames


def draw_disc(canvas, frame, cx, cy, radius, inner, outer):
    for y in range(int(cy - radius - 1), int(cy + radius + 2)):
        for x in range(int(cx - radius - 1), int(cx + radius + 2)):
            dx = x - cx
            dy = y - cy
            dist = math.sqrt(dx * dx + dy * dy)
            if dist <= radius:
                canvas.set_pixel(frame, x, y, blend(inner, outer, dist / max(radius, 1)))


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


def mix_colors(*colors):
    count = len(colors)
    return (
        sum(color[0] for color in colors) // count,
        sum(color[1] for color in colors) // count,
        sum(color[2] for color in colors) // count,
    )


def shift_cloud_parts(parts, dy):
    return [(cx, cy + dy, rx, ry, light, mid, dark, outline) for cx, cy, rx, ry, light, mid, dark, outline in parts]


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
    # Offset phase by PI/2 so frame 0 (phase=0) corresponds to sin(PI/2) = 1 (max pulse)
    effective_phase = phase + math.pi / 2.0
    pulse = 0.5 + 0.5 * math.sin(effective_phase)
    rotation = phase * 0.22
    
    # Sun core also pulses slightly
    draw_sun_orb(canvas, frame, cx, cy, core_radius + (pulse - 0.5) * 0.9 * pulse_strength, phase)
    
    # Large rays: thicker at base when pulsing
    # Base width varies from 1.8 to 2.4 based on pulse
    long_width = 1.8 + (pulse * 0.6 * pulse_strength)
    for idx in range(8):
        angle = rotation + idx * (math.pi / 4.0)
        draw_tapered_ray(canvas, frame, cx, cy, angle, core_radius + 1.8, max(2, round(long_ray_length * ray_scale)), long_width, blend((0, 0, 0), SUN_RAY_HOT, ray_alpha))
    
    # Small rays: remain thin (1.2) but get significantly longer when pulsing
    # Base length varies significantly with pulse
    short_scale = (short_ray_base + pulse * 3.5 * pulse_strength) * ray_scale
    for idx in range(8):
        # Position them exactly between the 8 main rays
        angle = rotation + (idx + 0.5) * (math.pi / 4.0)
        # Keep width at 1.2 constant
        draw_tapered_ray(canvas, frame, cx, cy, angle, core_radius + 1.2, max(2, round(short_scale)), 1.2, blend((0, 0, 0), SUN_RAY, ray_alpha * 0.92))


def draw_moon_patch(canvas, frame, cx, cy, patch_cx, patch_cy, rx, ry, base_mix, shade_strength, highlight_strength, wobble=0.0):
    for y in range(int(patch_cy - ry - 1), int(patch_cy + ry + 2)):
        for x in range(int(patch_cx - rx - 1), int(patch_cx + rx + 2)):
            dx = (x - patch_cx) / max(rx, 1)
            dy = (y - patch_cy) / max(ry, 1)
            ripple = math.sin(dx * 4.4 + wobble) * 0.08 + math.cos(dy * 5.1 - wobble * 0.7) * 0.06
            dist_sq = dx * dx + dy * dy + ripple
            if dist_sq > 1.0:
                continue
            moon_dx = x - cx
            moon_dy = y - cy
            if math.sqrt(moon_dx * moon_dx + moon_dy * moon_dy) > 9.7:
                continue

            density = (1.0 - max(0.0, dist_sq)) ** 0.95
            sphere_shadow = max(0.0, min(1.0, (moon_dx + moon_dy * 0.45 + 9.0) / 18.0))
            body = blend(MOON_FACE, MOON_CRATER, base_mix)
            body = blend(body, MOON_CRATER_SHADE, shade_strength * (0.40 + sphere_shadow * 0.60))
            canvas.blend_pixel(frame, x, y, body, density * 0.55)

            inner_highlight = max(0.0, 1.0 - ((dx - 0.16) ** 2 * 3.1 + (dy - 0.10) ** 2 * 3.1))
            if inner_highlight > 0.0:
                canvas.blend_pixel(frame, x, y, MOON_FACE, inner_highlight * highlight_strength)


def draw_moon_crater_basin(canvas, frame, cx, cy, crater_cx, crater_cy, rx, ry):
    for y in range(int(crater_cy - ry - 1), int(crater_cy + ry + 2)):
        for x in range(int(crater_cx - rx - 1), int(crater_cx + rx + 2)):
            dx = (x - crater_cx) / max(rx, 1)
            dy = (y - crater_cy) / max(ry, 1)
            dist_sq = dx * dx + dy * dy
            if dist_sq > 1.0:
                continue
            moon_dx = x - cx
            moon_dy = y - cy
            if math.sqrt(moon_dx * moon_dx + moon_dy * moon_dy) > 9.7:
                continue

            dist = math.sqrt(dist_sq)
            rim = max(0.0, 1.0 - abs(dist - 0.84) / 0.18)
            bowl = (1.0 - dist) ** 1.3
            light_dir = max(0.0, min(1.0, ((-dx) * 0.7 + (-dy) * 0.6 + 1.0) * 0.5))
            shadow_dir = max(0.0, min(1.0, (dx * 0.9 + dy * 0.5 + 1.0) * 0.5))

            crater_color = blend(MOON_CRATER, MOON_CRATER_SHADE, 0.55 + shadow_dir * 0.35)
            crater_color = blend(crater_color, MOON_FACE, bowl * 0.18)
            canvas.blend_pixel(frame, x, y, crater_color, 0.26 + rim * 0.30 + bowl * 0.18)

            if rim > 0.0:
                canvas.blend_pixel(frame, x, y, MOON_CRATER_SHADE, rim * 0.26)
                canvas.blend_pixel(frame, x, y, MOON_FACE, rim * light_dir * 0.14)


def draw_moon_orb(canvas, frame, cx, cy, radius, phase):
    glow_radius = radius + 2.8
    draw_soft_glow(canvas, frame, cx, cy, glow_radius, glow_radius, MOON_HALO, 0.11)
    for y in range(int(cy - glow_radius - 1), int(cy + glow_radius + 2)):
        for x in range(int(cx - glow_radius - 1), int(cx + glow_radius + 2)):
            dx = x - cx
            dy = y - cy
            dist = math.sqrt(dx * dx + dy * dy)
            if dist <= radius:
                radial = dist / max(radius, 1)
                base = blend(MOON_FACE, MOON_RIM, radial ** 1.05)
                sphere_light = max(0.0, min(1.0, (-dx * 0.62 - dy * 0.74 + radius * 1.05) / (radius * 2.1)))
                sphere_shadow = max(0.0, min(1.0, (dx * 0.52 + dy * 0.68 + radius * 0.85) / (radius * 2.0)))
                shine_dx = (dx + radius * 0.30) / max(radius, 1)
                shine_dy = (dy + radius * 0.36) / max(radius, 1)
                shine = max(0.0, 1.0 - (shine_dx * shine_dx + shine_dy * shine_dy) * 1.55)
                color = blend(base, MOON_FACE, sphere_light * 0.42 + shine * 0.28)
                color = blend(color, MOON_RIM, sphere_shadow * 0.30)
                color = blend(color, MOON_CRATER, max(0.0, radial - 0.55) * 0.10)
                canvas.set_pixel(frame, x, y, color)
            elif dist <= glow_radius:
                halo_t = 1.0 - (dist - radius) / max(glow_radius - radius, 1)
                canvas.blend_pixel(frame, x, y, MOON_HALO, halo_t * 0.07)

    maria = [
        (-0.8, -0.4, 4.7, 3.9, 0.42, 0.64, 0.10, 0.4),
        (-4.0, 1.0, 2.5, 2.0, 0.48, 0.72, 0.08, 1.5),
        (4.1, 2.0, 2.4, 1.8, 0.44, 0.68, 0.08, 2.2),
        (-0.4, 5.0, 3.9, 1.8, 0.46, 0.76, 0.07, 0.9),
        (1.8, -4.2, 2.1, 1.6, 0.32, 0.56, 0.12, 1.1),
        (-5.8, -1.1, 1.6, 2.5, 0.54, 0.78, 0.06, 2.9),
    ]
    for off_x, off_y, rx, ry, base_mix, shade_strength, highlight_strength, wobble in maria:
        draw_moon_patch(canvas, frame, cx, cy, cx + off_x, cy + off_y, rx, ry, base_mix, shade_strength, highlight_strength, wobble)

    craters = [
        (-2.8, -1.6, 2.2, 2.0),
        (3.6, 0.1, 2.0, 1.8),
        (0.4, 4.3, 2.5, 2.0),
        (-5.7, 2.1, 1.5, 1.9),
        (4.9, -3.2, 1.4, 1.2),
    ]
    for off_x, off_y, rx, ry in craters:
        draw_moon_crater_basin(canvas, frame, cx, cy, cx + off_x, cy + off_y, rx, ry)


def draw_breathing_moon(canvas, frame, cx, cy, radius, phase, pulse_strength=1.0):
    draw_moon_orb(canvas, frame, cx, cy, radius, phase)


def draw_cloud_shape(canvas, frame, parts, phase=0.0, morph_x=0.0, morph_y=0.0):
    for idx, (cx, cy, rx, ry, light, mid, dark, outline) in enumerate(parts):
        local_phase = phase + idx * 0.75
        animated_rx = rx + morph_x * math.sin(local_phase)
        animated_ry = ry + morph_y * math.cos(local_phase * 0.95)
        for y in range(int(cy - animated_ry - 1), int(cy + animated_ry + 2)):
            for x in range(int(cx - animated_rx - 1), int(cx + animated_rx + 2)):
                dx = (x - cx) / max(animated_rx, 1)
                dy = (y - cy) / max(animated_ry, 1)
                dist_sq = dx * dx + dy * dy
                if dist_sq <= 1.0:
                    rim = dist_sq ** 0.80
                    depth = max(0.0, min(1.0, (dy + 0.18) * 0.72 + rim * 0.62))
                    base = blend(mid, dark, min(1.0, rim * 0.72 + depth * 0.58))
                    highlight = max(0.0, 1.0 - ((dx + 0.55) ** 2 + (dy + 0.58) ** 2) * 1.7)
                    shadow_pocket = max(0.0, 1.0 - ((dx - 0.08) ** 2 * 3.2 + (dy - 0.18) ** 2 * 4.8))
                    color = blend(base, dark, shadow_pocket * 0.34)
                    color = blend(color, light, highlight * 1.08)
                    if dist_sq > 0.72:
                        outer_mix = min(1.0, (dist_sq - 0.72) / 0.28)
                        guard = blend(dark, outline, 0.35)
                        color = blend(color, guard, outer_mix)
                    if dist_sq > 0.90:
                        ring_mix = min(1.0, (dist_sq - 0.90) / 0.10)
                        guard = blend(dark, outline, 0.12)
                        color = blend(color, guard, 0.85 * ring_mix)
                    canvas.set_pixel(frame, x, y, color)


def draw_cloud_inner_tint(canvas, frame, phase, bounds, dark_color, light_color, strength):
    x0, x1, y0, y1, cx, cy, rx, ry = bounds
    for y in range(y0, y1):
        for x in range(x0, x1):
            if canvas.sample(frame, x, y) == (0, 0, 0):
                continue
            dx = (x - cx) / max(rx, 1)
            dy = (y - cy) / max(ry, 1)
            density = max(0.0, 1.0 - (dx * dx + dy * dy))
            if density <= 0.0:
                continue
            pulse = 0.5 + 0.5 * math.sin(phase + x * 0.16 - y * 0.12)
            relief = density * (0.28 + pulse * 0.42)
            tint = blend(dark_color, light_color, relief)
            opacity = density * strength
            if y >= cy:
                opacity *= 1.12
            canvas.blend_pixel(frame, x, y, tint, opacity)


def draw_drop(canvas, frame, x, top_y, shape_id=0):
    shapes = {
        0: [
            (0, 0, (242, 248, 255)), (1, 0, (186, 224, 255)),
            (0, 1, (144, 204, 255)), (1, 1, (74, 142, 232)),
            (0, 2, (90, 156, 234)), (1, 2, (28, 88, 194)),
        ],
        1: [
            (0, 0, (246, 250, 255)), (1, 0, (204, 230, 255)),
            (0, 1, (156, 210, 255)), (1, 1, (92, 154, 238)),
            (0, 2, (100, 164, 236)), (1, 2, (46, 102, 204)),
            (0, 3, (74, 126, 220)), (1, 3, (22, 72, 176)),
        ],
        2: [
            (0, 0, (246, 250, 255)), (1, 0, (216, 236, 255)), (2, 0, (164, 210, 252)),
            (0, 1, (170, 218, 255)), (1, 1, (112, 172, 242)), (2, 1, (54, 116, 214)),
        ],
        3: [
            (0, 0, (244, 248, 255)),
            (0, 1, (164, 214, 255)),
            (0, 2, (84, 148, 234)),
        ],
        4: [
            (0, 0, (246, 250, 255)), (1, 0, (210, 234, 255)),
            (0, 1, (164, 214, 255)), (1, 1, (94, 156, 238)),
            (0, 2, (118, 176, 242)), (1, 2, (48, 104, 204)),
            (0, 3, (74, 128, 220)),
        ],
    }
    shape = shapes[shape_id % len(shapes)]
    for dx, dy, color in shape:
        canvas.set_pixel(frame, x + dx, top_y + dy, color)


def draw_rain_drop(canvas, frame, x, top_y, variant):
    shapes = {
        "small_2": [
            (0, 0, (244, 248, 255)),
            (0, 1, (98, 162, 236)),
        ],
        "small_3": [
            (0, 0, (246, 250, 255)),
            (0, 1, (156, 214, 255)),
            (0, 2, (66, 128, 220)),
        ],
        "big_3": [
            (0, 0, (246, 250, 255)),
            (0, 1, (170, 220, 255)),
            (1, 1, (126, 188, 246)),
            (0, 2, (82, 144, 228)),
            (1, 2, (40, 98, 198)),
        ],
        "big_4": [
            (0, 0, (246, 250, 255)),
            (0, 1, (188, 228, 255)),
            (1, 1, (150, 204, 248)),
            (0, 2, (110, 174, 238)),
            (1, 2, (74, 136, 224)),
            (0, 3, (56, 114, 212)),
            (1, 3, (28, 80, 184)),
        ],
    }
    for dx, dy, color in shapes[variant]:
        canvas.set_pixel(frame, x + dx, top_y + dy, color)


def draw_rain_streak(canvas, frame, x, top_y, color):
    streak = [(0, 0, 0.70), (1, 1, 1.00), (1, 2, 0.85)]
    for dx, dy, local_strength in streak:
        canvas.add_pixel(frame, x + dx, top_y + dy, color, local_strength)


def draw_glow(canvas, frame, cx, cy, radius_x, radius_y, color, strength):
    for y in range(int(cy - radius_y), int(cy + radius_y + 1)):
        for x in range(int(cx - radius_x), int(cx + radius_x + 1)):
            dx = (x - cx) / max(radius_x, 1)
            dy = (y - cy) / max(radius_y, 1)
            dist_sq = dx * dx + dy * dy
            if dist_sq <= 1.0:
                canvas.blend_pixel(frame, x, y, color, (1.0 - dist_sq) * strength)


def draw_lightning(canvas, frame, points, bright, core, thick):
    for index in range(len(points) - 1):
        x1, y1 = points[index]
        x2, y2 = points[index + 1]
        steps = max(abs(x2 - x1), abs(y2 - y1), 1)
        for step in range(steps + 1):
            t = step / steps
            x = round(x1 + (x2 - x1) * t)
            y = round(y1 + (y2 - y1) * t)
            if thick:
                for ox in range(-1, 2):
                    for oy in range(-1, 2):
                        dist = abs(ox) + abs(oy)
                        if dist <= 1:
                            canvas.add_pixel(frame, x + ox, y + oy, bright, 1.0 if dist == 0 else 0.85)
            else:
                canvas.add_pixel(frame, x, y, bright, 0.95)
                if step % 2 == 0:
                    canvas.add_pixel(frame, x + 1, y, bright, 0.55)
            canvas.add_pixel(frame, x, y, core, 1.0)
            if thick and y + 1 < canvas.height:
                canvas.add_pixel(frame, x, y + 1, core, 0.92)


def draw_flake(canvas, frame, x, y, shape_id, tone):
    shapes = {
        0: [((0, 0), (255, 255, 255))],
        1: [
            ((0, -1), (240, 240, 240)),
            ((-1, 0), (246, 246, 246)),
            ((0, 0), (255, 255, 255)),
            ((1, 0), (246, 246, 246)),
            ((0, 1), (240, 240, 240)),
        ],
        2: [
            ((0, -1), (242, 242, 242)),
            ((-1, 0), (248, 248, 248)),
            ((1, 0), (248, 248, 248)),
            ((0, 1), (242, 242, 242)),
        ],
        3: [
            ((-1, -1), (236, 236, 236)),
            ((1, -1), (242, 242, 242)),
            ((0, 0), (255, 255, 255)),
            ((-1, 1), (242, 242, 242)),
            ((1, 1), (236, 236, 236)),
        ],
        4: [
            ((-1, -1), (240, 240, 240)),
            ((1, -1), (246, 246, 246)),
            ((-1, 1), (246, 246, 246)),
            ((1, 1), (240, 240, 240)),
        ],
        5: [
            ((-1, -1), (248, 248, 248)),
            ((1, -1), (248, 248, 248)),
            ((-1, 1), (248, 248, 248)),
            ((1, 1), (248, 248, 248)),
        ],
    }
    for (dx, dy), color in shapes[shape_id % len(shapes)]:
        px = x + dx
        py = y + dy
        if py < 0:
            continue
        if py >= canvas.height:
            continue
        canvas.blend_pixel(frame, px, py, color, 1.0)


def draw_fog_ribbon(canvas, frame, phase, center_y, amp, thickness, color, highlight):
    for x in range(1, canvas.width - 1):
        wave_y = center_y + math.sin(phase + x * 0.22) * amp + math.sin(phase * 0.6 + x * 0.09) * (amp * 0.45)
        for y in range(max(0, int(wave_y - thickness - 1)), min(canvas.height, int(wave_y + thickness + 2))):
            dist = abs(y - wave_y) / max(thickness, 1)
            if dist <= 1.0:
                opacity = (1.0 - dist) ** 1.4
                canvas.blend_pixel(frame, x, y, color, opacity * 0.34)
                if y <= wave_y:
                    canvas.blend_pixel(frame, x, y, highlight, opacity * 0.18)


def draw_fog_puff(canvas, frame, cx, cy, rx, ry, color, highlight):
    for y in range(int(cy - ry - 1), int(cy + ry + 2)):
        for x in range(int(cx - rx - 1), int(cx + rx + 2)):
            dx = (x - cx) / max(rx, 1)
            dy = (y - cy) / max(ry, 1)
            dist_sq = dx * dx + dy * dy
            if dist_sq <= 1.0:
                softness = (1.0 - dist_sq) ** 1.15
                canvas.blend_pixel(frame, x, y, color, softness * 0.24)
                light = max(0.0, 1.0 - ((dx + 0.45) ** 2 + (dy + 0.35) ** 2) * 1.8)
                canvas.blend_pixel(frame, x, y, highlight, light * 0.12)


def draw_distant_tree(canvas, frame, cx, base_y, trunk_h, canopy_w, trunk_color, canopy_dark, canopy_light):
    trunk_top = base_y - trunk_h
    for y in range(trunk_top, base_y):
        for x in range(cx - 1, cx + 1):
            fade = 0.42 + 0.16 * (y - trunk_top) / max(trunk_h, 1)
            canvas.blend_pixel(frame, x, y, trunk_color, fade)

    layers = [
        (base_y - trunk_h - 5, canopy_w * 0.52),
        (base_y - trunk_h - 8, canopy_w * 0.38),
        (base_y - trunk_h - 11, canopy_w * 0.24),
    ]
    for cy, rx in layers:
        ry = rx * 0.82
        for y in range(int(cy - ry - 1), int(cy + ry + 2)):
            for x in range(int(cx - rx - 1), int(cx + rx + 2)):
                dx = (x - cx) / max(rx, 1)
                dy = (y - cy) / max(ry, 1)
                dist_sq = dx * dx + dy * dy
                if dist_sq <= 1.0:
                    body = blend(canopy_light, canopy_dark, min(1.0, dist_sq ** 0.75))
                    opacity = (1.0 - dist_sq) ** 1.15
                    canvas.blend_pixel(frame, x, y, body, opacity * 0.36)

    for y in range(base_y - trunk_h - 9, base_y - trunk_h + 1):
        halo = 1.0 - abs(y - (base_y - trunk_h - 4)) / 6.0
        if halo <= 0.0:
            continue
        canvas.blend_pixel(frame, cx, y, canopy_light, halo * 0.10)


def draw_backlight_halo(canvas, frame, cx, cy, rx, ry, color, strength):
    for y in range(int(cy - ry - 1), int(cy + ry + 2)):
        for x in range(int(cx - rx - 1), int(cx + rx + 2)):
            dx = (x - cx) / max(rx, 1)
            dy = (y - cy) / max(ry, 1)
            dist_sq = dx * dx + dy * dy
            if dist_sq <= 1.0:
                falloff = (1.0 - dist_sq) ** 1.7
                canvas.blend_pixel(frame, x, y, color, falloff * strength)


def draw_fog_cartoon_swirl(canvas, frame, cx, cy, rx, ry, phase, body, shade, highlight):
    for y in range(int(cy - ry - 2), int(cy + ry + 3)):
        for x in range(int(cx - rx - 2), int(cx + rx + 3)):
            dx = (x - cx) / max(rx, 1)
            dy = (y - cy) / max(ry, 1)
            wave = math.sin(phase + dx * 4.2) * 0.10 + math.cos(phase * 0.6 + dy * 5.0) * 0.08
            dist_sq = dx * dx + dy * dy + wave
            if dist_sq <= 1.0:
                density = (1.0 - max(0.0, dist_sq)) ** 1.05
                roll = max(0.0, 1.0 - ((dx + 0.42) ** 2 * 2.8 + (dy + 0.18) ** 2 * 3.6))
                curl = max(0.0, 1.0 - ((dx - 0.30) ** 2 * 6.4 + (dy - 0.02) ** 2 * 7.5))
                color = blend(body, shade, min(1.0, (dx + 1.0) * 0.32 + (dy + 1.0) * 0.16))
                color = blend(color, shade, curl * 0.55)
                color = blend(color, highlight, roll * 0.90)
                canvas.blend_pixel(frame, x, y, color, density * 0.48)


def draw_fog_cartoon_band(canvas, frame, phase, center_y, amp, thickness, body, shade, highlight):
    for x in range(0, canvas.width):
        wave_y = center_y + math.sin(phase + x * 0.25) * amp + math.cos(phase * 0.8 + x * 0.12) * (amp * 0.42)
        for y in range(max(0, int(wave_y - thickness - 2)), min(canvas.height, int(wave_y + thickness + 3))):
            dist = abs(y - wave_y) / max(thickness, 1)
            if dist <= 1.0:
                opacity = (1.0 - dist) ** 1.25
                curl = 0.5 + 0.5 * math.sin(phase * 1.3 + x * 0.34 + y * 0.28)
                color = blend(body, shade, curl * 0.42)
                canvas.blend_pixel(frame, x, y, color, opacity * 0.30)
                if y <= wave_y:
                    canvas.blend_pixel(frame, x, y, highlight, opacity * 0.20)


def generate_clearsky(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, frames=10):
    canvas = SpriteCanvas(width, height, frames)
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_rotating_sun(canvas, frame, 16, 15, 7.74, phase, pulse_strength=0.8, long_ray_length=5.8, short_ray_base=2.9, ray_scale=0.9, ray_alpha=0.92)
    return finish_sprite(canvas)



def lerp(start, end, t):
    return start + (end - start) * t

def ease_in_out(t):
    t = max(0.0, min(1.0, t))
    return t * t * (3.0 - 2.0 * t)

def flight_lift(t, jolt=False):
    base = -math.sin(t * math.pi * 2.0) * 1.0
    if jolt:
        base -= math.sin(t * math.pi * 3.0) * 1.5
    return base

def draw_bat(canvas, frame, cx, cy, span, flap_phase=0.0, yaw=0.0):
    if span < 1: return
    color = (0, 0, 0)
    cx, cy, span = float(cx), float(cy), max(3.0, float(span))
    
    flap_val = math.sin(flap_phase) # -1.0 to 1.0 (1.0 = wings down, -1.0 = wings up)
    
    # Procedural generation matching the reference images
    if span <= 5.0:
        # Smallest bat (dash/dot)
        for dx in (0, 1, -1):
            canvas.set_pixel(frame, int(cx + dx), int(cy), color)
        if flap_val < -0.5:
            canvas.set_pixel(frame, int(cx - 1), int(cy - 1), color)
            canvas.set_pixel(frame, int(cx + 1), int(cy - 1), color)
        elif flap_val > 0.5:
            canvas.set_pixel(frame, int(cx - 1), int(cy + 1), color)
            canvas.set_pixel(frame, int(cx + 1), int(cy + 1), color)
        return

    if span <= 12.0:
        # Medium bat (pixel art style)
        r = span * 0.15
        for y in range(int(cy - r), int(cy + r) + 1):
            for x in range(int(cx - r), int(cx + r) + 1):
                if (x-cx)**2 + (y-cy)**2 <= r**2:
                    canvas.set_pixel(frame, x, y, color)
        
        # Ears
        ear_h = span * 0.2
        canvas.set_pixel(frame, int(cx - r), int(cy - ear_h), color)
        canvas.set_pixel(frame, int(cx + r), int(cy - ear_h), color)
        
        # Wings
        wing_w = span * 0.5
        y_w = cy - flap_val * span * 0.3
        steps = int(wing_w)
        for i in range(1, steps + 1):
            t = i / float(steps)
            wx = cx + i
            wy = cy + (y_w - cy) * t
            canvas.set_pixel(frame, int(wx), int(wy), color)
            canvas.set_pixel(frame, int(wx), int(wy + 1), color)
            wx = cx - i
            canvas.set_pixel(frame, int(wx), int(wy), color)
            canvas.set_pixel(frame, int(wx), int(wy + 1), color)
        return

    # Large bat (32px style)
    # Body
    body_w, body_h = span * 0.12, span * 0.18
    for y in range(int(cy - body_h), int(cy + body_h) + 1):
        for x in range(int(cx - body_w), int(cx + body_w) + 1):
            if (x-cx)**2 / max(1, body_w**2) + (y-cy)**2 / max(1, body_h**2) <= 1.0:
                canvas.set_pixel(frame, x, y, color)
                
    # Head and Ears
    head_r = span * 0.12
    head_y = cy - body_h * 0.7
    for y in range(int(head_y - head_r), int(head_y + head_r) + 1):
        for x in range(int(cx - head_r), int(cx + head_r) + 1):
            if (x-cx)**2 + (y-head_y)**2 <= head_r**2:
                canvas.set_pixel(frame, x, y, color)
                
    ear_w = span * 0.08
    ear_h = span * 0.25
    ear_spread = span * 0.12
    for side in (-1, 1):
        ex = cx + side * ear_spread
        ey = head_y - head_r * 0.2
        for y in range(int(ey - ear_h), int(ey) + 1):
            for x in range(int(ex - ear_w), int(ex + ear_w) + 1):
                # triangle
                if abs(x - ex) <= ear_w * (y - (ey - ear_h)) / max(1, ear_h):
                    canvas.set_pixel(frame, x, y, color)

    # Detailed Wings (based on 4 frames from second image)
    wing_span = span * 0.5 * (1.0 - abs(yaw)*0.5)
    steps = int(wing_span)
    scallop_depth = span * 0.12
    
    for side in (-1, 1):
        if yaw * side < -0.5: continue # Hidden by yaw
        
        shoulder_y = cy - body_h * 0.3
        
        if flap_val > 0.5: # Wings Down
            elbow_x = cx + side * wing_span * 0.4
            elbow_y = cy + span * 0.1
            tip_y = cy + span * 0.3
        elif flap_val > -0.5: # Wings Mid
            elbow_x = cx + side * wing_span * 0.5
            elbow_y = cy - span * 0.1
            tip_y = cy + span * 0.1
        else: # Wings Up
            elbow_x = cx + side * wing_span * 0.6
            elbow_y = cy - span * 0.3
            tip_y = cy - span * 0.4
            
        tip_x = cx + side * wing_span
        
        for i in range(1, steps + 1):
            t = i / float(steps)
            wx = cx + side * i
            
            if t < 0.5:
                nt = t / 0.5
                wy = shoulder_y + (elbow_y - shoulder_y) * nt
            else:
                nt = (t - 0.5) / 0.5
                wy = elbow_y + (tip_y - elbow_y) * nt
                
            scallop = abs(math.sin(t * math.pi * 3)) * scallop_depth
            bottom_y = wy + span * 0.15 - scallop
            
            for y in range(int(min(wy, bottom_y)), int(max(wy, bottom_y)) + 1):
                canvas.set_pixel(frame, round(wx), y, color)


def generate_clearsky_night(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, frames=50):
    canvas = SpriteCanvas(width, height, frames)
    moon_radius = 14.0
    moon_cx, moon_cy = 16.0, 16.0
    
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_breathing_moon(canvas, frame, moon_cx, moon_cy, moon_radius, phase * 0.5, pulse_strength=0.15)
        
        if frame == 0: continue
            
        full_span = 28.0
        
        # Total frames = 49 (1 to 49)
        # 1-13 (13 frames) Entry to other side
        if frame <= 13:
            t = (frame - 1) / 12.0
            cx = lerp(-4.0, 26.0, ease_in_out(t))
            # soubresaut
            bump = math.sin(t * math.pi) * -2.0 - math.exp(-((t-0.3)/0.1)**2) * 2.5
            cy = moon_cy + 2.0 + bump
            span = lerp(3.0, 10.0, t)
            yaw = 1.0 
            flap_phase = frame * 1.5
            draw_bat(canvas, frame, cx, cy, span, flap_phase, yaw=yaw)
            
        # 14-23 (10 frames) Returns to center, grows to full
        elif frame <= 23:
            t = (frame - 13) / 10.0
            cx = lerp(26.0, 16.0, ease_in_out(t))
            cy = lerp(moon_cy + 2.0, moon_cy, t) - math.sin(t * math.pi) * 1.5
            span = lerp(10.0, full_span, ease_in_out(t))
            yaw = lerp(1.0, 0.0, t)
            flap_phase = 13 * 1.5 + t * math.pi * 1.5
            draw_bat(canvas, frame, cx, cy, span, flap_phase, yaw=yaw)
            
        # 24-37 (14 frames) Hold center, exactly 2 wing beats
        elif frame <= 37:
            t = (frame - 23) / 14.0
            cx = moon_cx
            base_flap = 13 * 1.5 + math.pi * 1.5
            flap_phase = base_flap + t * math.pi * 4.0
            cy = moon_cy + math.sin(flap_phase) * 1.5
            draw_bat(canvas, frame, cx, cy, full_span, flap_phase, yaw=0.0)
            
        # 38-43 (6 frames) Leaves towards entry (left), shrinks to 16px
        elif frame <= 43:
            t = (frame - 37) / 6.0
            cx = lerp(16.0, 6.0, ease_in_out(t))
            bump = -math.exp(-((t-0.5)/0.2)**2) * 2.5
            cy = moon_cy + bump
            span = lerp(full_span, 16.0, ease_in_out(t))
            yaw = lerp(0.0, -1.0, t) 
            base_flap = 13*1.5 + math.pi*1.5 + 4.0*math.pi
            flap_phase = base_flap + t * math.pi * 2.5
            draw_bat(canvas, frame, cx, cy, span, flap_phase, yaw=yaw)
            
        # 44-49 (6 frames) Crosses moon to right, shrinks to 3px
        else:
            t = (frame - 43) / 6.0
            cx = lerp(6.0, 36.0, t**1.5)
            cy = lerp(moon_cy, moon_cy + 3.0, t) - math.sin(t * math.pi)*1.5
            span = lerp(16.0, 3.0, t)
            yaw = lerp(-1.0, 1.0, min(1.0, t*2.5))
            base_flap = 13*1.5 + math.pi*1.5 + 4.0*math.pi + 2.5*math.pi
            flap_phase = base_flap + t * math.pi * 2.5
            draw_bat(canvas, frame, cx, cy, span, flap_phase, yaw=yaw)
            
    return finish_sprite(canvas)


def generate_fair(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, frames=10):
    canvas = SpriteCanvas(width, height, frames)
    palette = (
        mix_colors(RAIN_CLOUD_LIGHT, (255, 255, 255)),
        mix_colors(RAIN_CLOUD_MID, (210, 222, 244)),
        mix_colors(RAIN_CLOUD_MID, RAIN_CLOUD_BASE),
        mix_colors(RAIN_CLOUD_BASE, RAIN_CLOUD_DARK),
    )
    parts = shift_cloud_parts([
        (11.0, 17.1, 4.9, 3.3, *palette),
        (17.6, 13.4, 6.1, 4.1, *palette),
        (23.3, 16.3, 4.8, 3.3, *palette),
        (18.0, 18.8, 8.0, 3.1, *palette),
    ], -5.0)
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_rotating_sun(canvas, frame, 11, 9, 6.12, phase, pulse_strength=0.65, long_ray_length=4.2, short_ray_base=2.0, ray_scale=0.68, ray_alpha=0.68)
        draw_cloud_shape(canvas, frame, parts, phase, morph_x=0.06, morph_y=0.05)
        draw_cloud_inner_tint(canvas, frame, phase, (10, 27, 5, 16, 18.0, 9.6, 6.0, 4.0), mix_colors(RAIN_CLOUD_MID, RAIN_CLOUD_BASE), palette[0], 0.15)
    return finish_sprite(canvas)


def generate_fair_night(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, frames=10):
    canvas = SpriteCanvas(width, height, frames)
    palette = (
        mix_colors(RAIN_CLOUD_LIGHT, (255, 255, 255)),
        mix_colors(RAIN_CLOUD_MID, (210, 222, 244)),
        mix_colors(RAIN_CLOUD_MID, RAIN_CLOUD_BASE),
        mix_colors(RAIN_CLOUD_BASE, RAIN_CLOUD_DARK),
    )
    parts = shift_cloud_parts([
        (12.6, 17.2, 4.1, 2.9, *palette),
        (18.8, 13.7, 5.3, 3.7, *palette),
        (24.2, 16.1, 4.1, 2.9, *palette),
        (19.2, 18.6, 6.9, 2.8, *palette),
    ], -5.0)
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_breathing_moon(canvas, frame, 11, 9, 6.85, phase, pulse_strength=0.7)
        draw_cloud_shape(canvas, frame, parts, phase, morph_x=0.06, morph_y=0.05)
        draw_cloud_inner_tint(canvas, frame, phase, (12, 28, 5, 15, 19.2, 9.4, 5.2, 3.6), mix_colors(RAIN_CLOUD_MID, RAIN_CLOUD_BASE), palette[0], 0.15)
    return finish_sprite(canvas)


def generate_partlycloudy(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, frames=10):
    canvas = SpriteCanvas(width, height, frames)
    palette = (
        mix_colors(RAIN_CLOUD_LIGHT, (232, 236, 246)),
        mix_colors(RAIN_CLOUD_MID, (126, 140, 180)),
        mix_colors(RAIN_CLOUD_DARK, (42, 50, 78)),
        mix_colors(RAIN_CLOUD_BASE, (18, 24, 40)),
    )
    parts = shift_cloud_parts([
        (9.2, 15.5, 6.1, 4.0, *palette),
        (16.0, 11.5, 7.5, 5.0, *palette),
        (22.7, 15.0, 6.0, 4.1, *palette),
        (16.0, 17.6, 9.8, 3.8, *palette),
    ], -4.5)
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_rotating_sun(canvas, frame, 10, 8, 6.3, phase * 1.15, pulse_strength=0.8, long_ray_length=4.3, short_ray_base=2.1, ray_scale=0.64, ray_alpha=0.60)
        draw_cloud_shape(canvas, frame, parts, phase, morph_x=0.08, morph_y=0.06)
        draw_cloud_inner_tint(canvas, frame, phase, (7, 26, 4, 16, 16.0, 9.3, 7.2, 4.8), palette[2], palette[0], 0.18)
    return finish_sprite(canvas)


def generate_partlycloudy_night(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, frames=10):
    canvas = SpriteCanvas(width, height, frames)
    palette = (
        mix_colors(RAIN_CLOUD_LIGHT, (232, 236, 246)),
        mix_colors(RAIN_CLOUD_MID, (126, 140, 180)),
        mix_colors(RAIN_CLOUD_DARK, (42, 50, 78)),
        mix_colors(RAIN_CLOUD_BASE, (18, 24, 40)),
    )
    parts = shift_cloud_parts([
        (9.2, 15.5, 6.1, 4.0, *palette),
        (16.0, 11.5, 7.5, 5.0, *palette),
        (22.7, 15.0, 6.0, 4.1, *palette),
        (16.0, 17.6, 9.8, 3.8, *palette),
    ], -4.5)
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_breathing_moon(canvas, frame, 10, 8, 7.0, phase * 1.15, pulse_strength=0.85)
        draw_cloud_shape(canvas, frame, parts, phase, morph_x=0.08, morph_y=0.06)
        draw_cloud_inner_tint(canvas, frame, phase, (7, 26, 4, 16, 16.0, 9.3, 7.2, 4.8), palette[2], palette[0], 0.18)
    return finish_sprite(canvas)


def generate_cloud(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, frames=15):
    canvas = SpriteCanvas(width, height, frames)
    parts = shift_cloud_parts([
        (8.7, 15.2, 6.2, 4.1, mix_colors(RAIN_CLOUD_MID, (176, 188, 220)), mix_colors(RAIN_CLOUD_DARK, (84, 98, 138)), mix_colors(RAIN_CLOUD_BASE, (16, 20, 34)), mix_colors(RAIN_CLOUD_BASE, (8, 10, 18))),
        (14.6, 11.0, 7.3, 4.9, mix_colors(RAIN_CLOUD_LIGHT, (210, 220, 244)), mix_colors(RAIN_CLOUD_MID, (110, 124, 170)), mix_colors(RAIN_CLOUD_BASE, (18, 22, 36)), mix_colors(RAIN_CLOUD_BASE, (8, 10, 18))),
        (22.9, 14.3, 6.4, 4.2, mix_colors(RAIN_CLOUD_MID, (168, 182, 218)), mix_colors(RAIN_CLOUD_DARK, (82, 96, 138)), mix_colors(RAIN_CLOUD_BASE, (16, 20, 34)), mix_colors(RAIN_CLOUD_BASE, (8, 10, 18))),
        (16.2, 17.0, 10.0, 4.0, mix_colors(RAIN_CLOUD_DARK, (116, 132, 174)), mix_colors(RAIN_CLOUD_BASE, (62, 76, 112)), mix_colors(RAIN_CLOUD_BASE, (14, 18, 28)), mix_colors(RAIN_CLOUD_BASE, (8, 10, 18))),
    ], -4.8)
    passing_showers = [
        {"start": 1.3, "x": 10.8, "shape": 0, "span": 13.8, "sway": 0.55, "phase": 0.1},
        {"start": 7.1, "x": 19.4, "shape": 1, "span": 12.6, "sway": 0.82, "phase": 1.9},
        {"start": 12.2, "x": 14.2, "shape": 2, "span": 14.1, "sway": 0.64, "phase": 3.2},
    ]
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_cloud_shape(canvas, frame, parts, phase, morph_x=0.10, morph_y=0.08)
        draw_cloud_inner_tint(canvas, frame, phase, (8, 25, 4, 15, 16.0, 8.8, 7.4, 4.8), mix_colors(RAIN_CLOUD_BASE, (40, 52, 84)), mix_colors(RAIN_CLOUD_LIGHT, (174, 188, 224)), 0.20)
        for shower in passing_showers:
            shower_progress = (frame - shower["start"]) % frames
            if shower_progress > 3.8:
                continue
            top_y = 11 + int((shower_progress / 3.8) * shower["span"])
            sway = math.sin((shower_progress / 3.8) * math.pi * 1.3 + shower["phase"]) * shower["sway"]
            drop_x = round(shower["x"] + sway)
            if 11 <= top_y <= 29:
                draw_drop(canvas, frame, drop_x, top_y, shower["shape"])
    return finish_sprite(canvas)


def generate_rain(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, normal_frames=10):
    total_frames = normal_frames + 2 * TRANSITION_FRAMES
    canvas = SpriteCanvas(width, height, total_frames)
    small_drop_start_y = 10.0
    large_drop_start_y = 11.0
    parts = shift_cloud_parts([
        (9.5, 14.0, 6.7, 4.4, (210, 222, 250), (108, 126, 182), (38, 55, 105), (184, 198, 232)),
        (22.5, 14.0, 6.7, 4.4, (210, 222, 250), (108, 126, 182), (38, 55, 105), (184, 198, 232)),
        (16.0, 10.8, 8.4, 5.6, (238, 244, 255), (148, 168, 220), (62, 80, 132), (214, 224, 246)),
        (16.0, 15.5, 10.2, 4.1, (176, 194, 236), (92, 112, 170), (34, 49, 96), (156, 176, 220)),
    ], -4.0)
    drops = [
        {"x": 6.0, "phase": 0.02, "variant": "small_2", "start_y": small_drop_start_y, "height": 2},
        {"x": 8.6, "phase": 0.16, "variant": "small_3", "start_y": small_drop_start_y, "height": 3},
        {"x": 11.2, "phase": 0.30, "variant": "big_3", "start_y": large_drop_start_y, "height": 3},
        {"x": 13.5, "phase": 0.08, "variant": "small_2", "start_y": small_drop_start_y, "height": 2},
        {"x": 15.7, "phase": 0.42, "variant": "big_4", "start_y": large_drop_start_y, "height": 4},
        {"x": 18.0, "phase": 0.56, "variant": "small_3", "start_y": small_drop_start_y, "height": 3},
        {"x": 20.3, "phase": 0.70, "variant": "big_3", "start_y": large_drop_start_y, "height": 3},
        {"x": 22.6, "phase": 0.24, "variant": "small_2", "start_y": small_drop_start_y, "height": 2},
        {"x": 24.8, "phase": 0.84, "variant": "big_4", "start_y": large_drop_start_y, "height": 4},
        {"x": 27.0, "phase": 0.50, "variant": "small_3", "start_y": small_drop_start_y, "height": 3},
    ]

    def _draw_rain_frame(frame_idx, frame_t, drop_filter_fn):
        phase = (frame_t / normal_frames) * 2.0 * math.pi
        draw_cloud_shape(canvas, frame_idx, parts, phase, morph_x=0.34, morph_y=0.26)
        draw_cloud_inner_tint(canvas, frame_idx, phase, (10, 23, 4, 11, 16.0, 7.8, 6.0, 3.7),
                              (96, 118, 180), (184, 204, 246), 0.55)
        for drop in drops:
            if not drop_filter_fn(drop):
                continue
            progress = (frame_t / normal_frames + drop["phase"]) % 1.0
            loop_distance = height - drop["start_y"] + drop["height"]
            top_y = round(drop["start_y"] + progress * loop_distance)
            if 10 <= top_y <= 31:
                draw_rain_drop(canvas, frame_idx, round(drop["x"]), top_y, drop["variant"])

    # Entry frames: drops fill in progressively (low-phase drops appear first)
    for e in range(TRANSITION_FRAMES):
        threshold = (e + 1) / TRANSITION_FRAMES
        _draw_rain_frame(e, e, lambda d, t=threshold: d["phase"] < t)

    # Normal frames: all drops, same as original behavior
    for n in range(normal_frames):
        _draw_rain_frame(TRANSITION_FRAMES + n, n, lambda d: True)

    # Exit frames: drops drain away (low-phase drops disappear first)
    for x in range(TRANSITION_FRAMES):
        threshold = x / TRANSITION_FRAMES
        frame_t = TRANSITION_FRAMES + x
        _draw_rain_frame(TRANSITION_FRAMES + normal_frames + x, frame_t, lambda d, t=threshold: d["phase"] >= t)

    data, _ = finish_sprite(canvas)
    return data, total_frames, TRANSITION_FRAMES


def generate_snow(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, normal_frames=15):
    total_frames = normal_frames + 2 * TRANSITION_FRAMES
    canvas = SpriteCanvas(width, height, total_frames)
    small_flake_start_y = 11.0
    large_flake_start_y = 13.0
    palette = (
        mix_colors(RAIN_CLOUD_LIGHT, (255, 255, 255)),
        mix_colors(RAIN_CLOUD_MID, (210, 222, 244)),
        mix_colors(RAIN_CLOUD_MID, RAIN_CLOUD_BASE),
        mix_colors(RAIN_CLOUD_BASE, RAIN_CLOUD_DARK),
    )
    parts = shift_cloud_parts([
        (9.2, 14.6, 6.1, 4.0, *palette),
        (16.0, 10.6, 7.5, 5.0, *palette),
        (22.7, 14.1, 6.0, 4.1, *palette),
        (16.0, 16.7, 9.8, 3.8, *palette),
    ], -4.5)
    flakes = [
        {"x": 8.8, "phase": 0.04, "shape": 0, "tone": (255, 255, 255), "drift": 0.0, "curve": "line", "release": -0.5, "speed": 0.5, "start_y": small_flake_start_y},
        {"x": 11.7, "phase": 0.18, "shape": 0, "tone": (255, 255, 255), "drift": 0.0, "curve": "line", "release": 0.4, "speed": 0.5, "start_y": small_flake_start_y},
        {"x": 14.4, "phase": 0.33, "shape": 0, "tone": (255, 255, 255), "drift": 0.0, "curve": "line", "release": -0.3, "speed": 0.5, "start_y": small_flake_start_y},
        {"x": 17.0, "phase": 0.09, "shape": 0, "tone": (255, 255, 255), "drift": 0.0, "curve": "line", "release": 0.3, "speed": 0.5, "start_y": small_flake_start_y},
        {"x": 20.1, "phase": 0.46, "shape": 0, "tone": (255, 255, 255), "drift": 0.0, "curve": "line", "release": -0.4, "speed": 0.5, "start_y": small_flake_start_y},
        {"x": 23.0, "phase": 0.61, "shape": 0, "tone": (255, 255, 255), "drift": 0.0, "curve": "line", "release": 0.5, "speed": 0.5, "start_y": small_flake_start_y},
        {"x": 11.8, "phase": 0.27, "shape": 1, "tone": (255, 255, 255), "drift": 0.14, "curve": "diag", "release": -0.6, "speed": 1.0, "start_y": large_flake_start_y},
        {"x": 14.8, "phase": 0.53, "shape": 3, "tone": (255, 255, 255), "drift": -0.12, "curve": "diag", "release": 0.6, "speed": 1.0, "start_y": large_flake_start_y},
        {"x": 17.3, "phase": 0.76, "shape": 2, "tone": (255, 255, 255), "drift": 0.13, "curve": "diag", "release": -0.5, "speed": 1.0, "start_y": large_flake_start_y},
        {"x": 20.2, "phase": 0.12, "shape": 4, "tone": (255, 255, 255), "drift": -0.11, "curve": "diag", "release": 0.5, "speed": 1.0, "start_y": large_flake_start_y},
    ]

    def _draw_snow_frame(frame_idx, frame_t, flake_filter_fn):
        phase = (frame_t / normal_frames) * 2.0 * math.pi
        draw_cloud_shape(canvas, frame_idx, parts, phase, morph_x=0.06, morph_y=0.05)
        draw_cloud_inner_tint(canvas, frame_idx, phase, (7, 26, 4, 15, 16.0, 7.6, 7.2, 4.8),
                              mix_colors(RAIN_CLOUD_MID, RAIN_CLOUD_BASE), palette[0], 0.15)
        for flake in flakes:
            if not flake_filter_fn(flake):
                continue
            progress = (frame_t / normal_frames * flake["speed"] + flake["phase"]) % 1.0
            loop_distance = height - flake["start_y"] + 4.0
            fall_y = flake["start_y"] + progress * loop_distance
            release_mix = max(0.0, 1.0 - progress * 3.2)
            release_x = math.sin(flake["phase"] * 12.0 + progress * math.pi * 2.0) * flake["release"] * release_mix
            if flake["curve"] == "diag":
                wind = (progress - 0.5) * 2.0 * flake["drift"]
                wind += math.sin(progress * math.pi * 2.0 + flake["x"] * 0.15) * 0.10
                drift_x = round(flake["x"] + wind + release_x)
            else:
                drift_x = round(flake["x"] + release_x)
            draw_flake(canvas, frame_idx, drift_x, round(fall_y), flake["shape"], flake["tone"])

    # Entry frames: flakes fill in progressively (low-phase flakes appear first)
    for e in range(TRANSITION_FRAMES):
        threshold = (e + 1) / TRANSITION_FRAMES
        _draw_snow_frame(e, e, lambda f, t=threshold: f["phase"] < t)

    # Normal frames: all flakes, same as original behavior
    for n in range(normal_frames):
        _draw_snow_frame(TRANSITION_FRAMES + n, n, lambda f: True)

    # Exit frames: flakes drain away (low-phase flakes disappear first)
    for x in range(TRANSITION_FRAMES):
        threshold = x / TRANSITION_FRAMES
        frame_t = TRANSITION_FRAMES + x
        _draw_snow_frame(TRANSITION_FRAMES + normal_frames + x, frame_t, lambda f, t=threshold: f["phase"] >= t)

    data, _ = finish_sprite(canvas)
    return data, total_frames, TRANSITION_FRAMES


def generate_storm(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, frames=20):
    canvas = SpriteCanvas(width, height, frames)
    parts = shift_cloud_parts([
        (9.2, 14.1, 6.5, 4.3, (154, 170, 212), (70, 84, 124), (16, 20, 34), (112, 130, 188)),
        (22.8, 14.1, 6.5, 4.3, (154, 170, 212), (70, 84, 124), (16, 20, 34), (112, 130, 188)),
        (16.0, 10.6, 8.6, 5.8, (196, 206, 240), (92, 108, 156), (22, 28, 46), (126, 144, 204)),
        (16.0, 16.0, 10.3, 4.0, (118, 132, 182), (48, 58, 92), (12, 14, 24), (92, 108, 160)),
    ], -3.5)
    flash_strengths = [
        0.00, 0.00, 0.00, 0.30, 0.00,
        0.00, 0.00, 0.00, 0.00, 0.00,
        0.00, 0.92, 0.00, 0.00, 0.24,
        0.00, 0.00, 0.00, 0.00, 0.00,
    ]
    bolt_shapes = [
        [(15, 7), (18, 11), (15, 14), (19, 18), (16, 24)],
        [(12, 8), (15, 12), (13, 15), (17, 18), (14, 23)],
        [(19, 8), (16, 12), (18, 15), (14, 19), (17, 24)],
    ]
    rain_columns = [8, 23]
    rain_offsets = [3, 11]
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        flash = flash_strengths[frame]
        flash_parts = []
        for cx, cy, rx, ry, light, mid, dark, outline in parts:
            flash_parts.append((
                cx,
                cy,
                rx,
                ry,
                blend(light, (255, 242, 120), flash * 0.35),
                blend(mid, (180, 176, 116), flash * 0.20),
                dark,
                blend(outline, (220, 230, 255), flash * 0.45),
            ))
        draw_cloud_shape(canvas, frame, flash_parts, phase, morph_x=0.28, morph_y=0.20)
        draw_cloud_inner_tint(canvas, frame, phase, (10, 23, 5, 13, 16.0, 9.7, 5.8, 3.9), (40, 50, 84), (116, 136, 196), 0.38)
        if flash > 0.0:
            bolt = bolt_shapes[frame % len(bolt_shapes)]
            glow_center_x = sum(point[0] for point in bolt) / len(bolt)
            glow_center_y = sum(point[1] for point in bolt) / len(bolt)
            is_yellow_flash = frame == 11
            glow_color = (250, 222, 88) if is_yellow_flash else (120, 184, 255)
            bolt_bright = (255, 240, 104) if is_yellow_flash else (78, 160, 255)
            bolt_core = (255, 252, 220) if is_yellow_flash else (210, 236, 255)
            draw_glow(canvas, frame, glow_center_x, glow_center_y, 9, 8, glow_color, flash * 0.32)
            draw_lightning(canvas, frame, bolt, bolt_bright, bolt_core, thick=is_yellow_flash)
        if frame in (4, 13, 18):
            step = height / frames
            for idx, column in enumerate(rain_columns):
                top_y = int((rain_offsets[idx] - 2 + frame * step) % height)
                if 11 <= top_y <= 27:
                    draw_rain_streak(canvas, frame, column, top_y, (86, 134, 230))
    return finish_sprite(canvas)


def generate_fog(width=SPRITE_WIDTH, height=SPRITE_HEIGHT, frames=12):
    canvas = SpriteCanvas(width, height, frames)
    upper_clouds = shift_cloud_parts([
        (6.6, 6.8, 4.9, 2.1, (220, 226, 232), (172, 180, 192), (116, 124, 138), (148, 156, 170)),
        (14.8, 5.6, 5.9, 2.4, (226, 232, 238), (178, 186, 198), (122, 130, 144), (154, 162, 176)),
        (24.4, 7.4, 5.1, 2.2, (216, 222, 230), (166, 174, 186), (110, 118, 132), (142, 150, 164)),
    ], -1.0)
    for frame in range(frames):
        phase = (frame / frames) * 2.0 * math.pi
        draw_soft_glow(canvas, frame, 16.0, 14.0, 13.0, 10.0, (188, 196, 208), 0.038)
        draw_cloud_shape(canvas, frame, upper_clouds, phase * 0.8, morph_x=0.08, morph_y=0.04)
        draw_cloud_inner_tint(canvas, frame, phase, (1, 31, 1, 10, 16.0, 5.7, 13.0, 4.2), (136, 144, 158), (226, 232, 238), 0.14)
        draw_fog_cartoon_band(canvas, frame, phase * 0.75 + 0.2, 8.0, 1.3, 2.6, (204, 210, 218), (156, 164, 176), (242, 244, 248))
        draw_fog_cartoon_band(canvas, frame, phase * 0.88 + 1.6, 14.0, 1.7, 3.2, (212, 218, 224), (162, 170, 182), (246, 248, 250))
        draw_fog_cartoon_band(canvas, frame, phase * 0.68 + 2.7, 20.6, 1.5, 3.0, (200, 206, 214), (150, 158, 170), (240, 242, 246))
        draw_fog_cartoon_swirl(canvas, frame, 8.5 + math.sin(phase * 0.7) * 1.0, 10.0, 6.1, 4.1, phase * 1.2, (208, 214, 220), (156, 164, 176), (246, 248, 250))
        draw_fog_cartoon_swirl(canvas, frame, 22.8 + math.sin(phase * 0.55 + 0.6) * 1.1, 12.5, 7.0, 4.6, phase * 1.1 + 1.4, (198, 204, 212), (146, 154, 166), (242, 244, 248))
        draw_fog_cartoon_swirl(canvas, frame, 15.4 + math.sin(phase * 0.82 + 1.3) * 0.9, 18.8, 9.4, 4.9, phase * 1.35 + 2.2, (216, 222, 228), (166, 174, 184), (248, 250, 252))
        draw_fog_cartoon_swirl(canvas, frame, 5.0 + math.sin(phase * 0.5 + 2.3) * 0.6, 22.0, 4.8, 3.1, phase * 1.5 + 0.8, (202, 208, 216), (150, 158, 168), (242, 244, 248))
        draw_fog_cartoon_swirl(canvas, frame, 27.2 + math.sin(phase * 0.6 + 1.8) * 0.7, 22.5, 5.2, 3.3, phase * 1.45 + 1.9, (202, 208, 216), (150, 158, 168), (242, 244, 248))
    return finish_sprite(canvas)


def save_to_c_file(filename, data, name, frames, width=SPRITE_WIDTH, height=SPRITE_HEIGHT, transition_frames=0):
    with open(filename, "w") as file:
        file.write("/*\n")
        file.write(" * AUTO-GENERATED FILE - DO NOT EDIT MANUALLY\n")
        file.write(" * This file was generated by tools/gen_weather_sprites.py\n")
        file.write(" * Any manual changes will be overwritten. To modify these sprites,\n")
        file.write(" * edit the drawing logic in the Python script and re-run it.\n")
        file.write(" */\n\n")
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


GENERATORS = {
    "sprite_clearsky": generate_clearsky,
    "sprite_clearsky_night": generate_clearsky_night,
    "sprite_fair": generate_fair,
    "sprite_fair_night": generate_fair_night,
    "sprite_partlycloudy": generate_partlycloudy,
    "sprite_partlycloudy_night": generate_partlycloudy_night,
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
        save_to_c_file(f"main/assets/{name}.c", data, name, frames, transition_frames=tr_frames)
        print(f"Generated main/assets/{name}.c")


def parse_args():
    parser = argparse.ArgumentParser(description="Generate Frixos animated weather sprites")
    parser.add_argument("sprites", nargs="*", choices=sorted(GENERATORS.keys()), help="Specific sprites to generate")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    generate_assets(args.sprites)