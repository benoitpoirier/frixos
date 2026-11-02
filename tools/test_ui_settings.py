#!/usr/bin/env python3
"""Host-side tests for every setting the web UI posts.

Runs the real SPIFFS scripts (no browser, no device) and checks the JSON they
POST. Then checks main/f-settings.c: a value the UI sends must survive both
validation and the apply path. Brightness time (p55/p56) and Wi-Fi hours
(p46/p47) are minutes from midnight, 0-1439. An apply gate of 0-23 accepts
the POST and then drops 07:30.

CI runs this file. It does not talk to hardware.
"""

from __future__ import annotations

import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SETTINGS_C = ROOT / "main" / "f-settings.c"
PWM_H = ROOT / "main" / "include" / "f-pwm.h"
ADAPTIVE_CORE_C = ROOT / "main" / "f-adaptive-core.c"
ADAPTIVE_RUNTIME_C = ROOT / "main" / "f-adaptive.c"
ADAPTIVE_TEST_C = ROOT / "tools" / "test_adaptive.c"
MAIN_C = ROOT / "main" / "main.c"
DISPLAY_C = ROOT / "main" / "f-display.c"
PWM_C = ROOT / "main" / "f-pwm.c"
CURVE_JS = ROOT / "spiffs" / "js" / "brightness-curve.js"

# What the harness types into the form, and what /api/settings must receive.
EXPECTED_SAVE = {
    "p00": "frixos-ui",
    "p34": "NewNet",
    "p35": "new-secret",
    "p03": 2,
    "p09": 1,
    "p36": 1,
    "p37": 1,
    "p60": "192.168.2.50",
    "p61": "192.168.2.1",
    "p62": "255.255.255.0",
    "p63": "8.8.8.8,8.8.4.4",
    "p17": "40.7128000",
    "p18": "-74.006000",
    "p19": "EST5EDT,M3.2.0,M11.1.0",
    "tz_iana": "America/New_York",
    "p46": 6 * 60 + 30,  # 06:30 Wi-Fi start
    "p47": 22 * 60,  # 22:00 Wi-Fi end
    "p22": 2,
    "p21": 20.5,
    "p20": 8.5,
    "p23": [60, 15],
    "p67": [0.5, 20, 5, 40, 50, 70, 500, 90],
    "p42": 250,
    "p43": 900,
    "p55": 7 * 60 + 30,  # 07:30 brightness-time start, not hour 7
    "p56": 22 * 60 + 45,  # 22:45 brightness-time end
    "p25": "http://ha.local:8123/",
    "p26": "new-token",
    "p27": 2,
    "p28": "new-key",
    "p29": 10,
    "p64": "KCASANFR123",
    "p65": "new-wu-key",
    "p66": 20,
    "p30": 1,
    "p31": "cgm-user",
    "p32": "glu-pass",
    "p33": 3,
    "p44": 2,
    "p45": 45,
    "p51": 180,
    "p52": 65,
    "p53": 1,
    "p54": "https://ns.example.com",
}

EXPECTED_RELOAD = {
    "dim_start": "07:00",  # stored p55 = 420
    "dim_end": "22:15",  # stored p56 = 1335
    "wifi_start": "08:00",  # stored p46 = 480
    "wifi_end": "00:00",
}

EXPECTED_SCREEN = {
    "p08": 1,
    "p24": 1,
    "p50": 1,
}


def fail(msg: str, failures: list[str]) -> None:
    failures.append(msg)


def same(a, b) -> bool:
    if isinstance(a, bool) or isinstance(b, bool):
        return a is b or a == b
    if isinstance(a, (int, float)) and isinstance(b, (int, float)):
        return float(a) == float(b)
    if isinstance(a, list) and isinstance(b, list):
        return len(a) == len(b) and all(same(x, y) for x, y in zip(a, b))
    return a == b


def run_harness(failures: list[str]) -> dict | None:
    try:
        proc = subprocess.run(
            ["node", str(ROOT / "tools" / "ui_settings_harness.mjs")],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=60,
            check=False,
        )
    except subprocess.TimeoutExpired:
        fail("UI harness timed out", failures)
        return None
    if proc.returncode != 0:
        detail = (proc.stderr or proc.stdout or "harness failed").strip()
        fail("UI harness exited %s\n%s" % (proc.returncode, detail), failures)
        return None
    line = next((ln for ln in proc.stdout.splitlines() if ln.startswith("RESULT ")), None)
    if not line:
        fail("UI harness produced no RESULT line\n" + proc.stdout + proc.stderr, failures)
        return None
    return json.loads(line[len("RESULT ") :])


def check_ui(observed: dict, failures: list[str]) -> None:
    reload = observed.get("reload") or {}
    for key, expected in EXPECTED_RELOAD.items():
        got = reload.get(key)
        if got != expected:
            fail("reload %s: expected %r, got %r" % (key, expected, got), failures)

    if observed.get("p35BeforeDirty"):
        fail("wifi password was posted before the user edited it", failures)

    save = observed.get("save")
    if not isinstance(save, dict):
        fail("Save did not POST /api/settings", failures)
        return
    for key, expected in EXPECTED_SAVE.items():
        if key not in save:
            fail("save omitted %s (expected %r)" % (key, expected), failures)
        elif not same(save[key], expected):
            fail("save %s: expected %r, got %r" % (key, expected, save[key]), failures)
    extra = sorted(set(save) - set(EXPECTED_SAVE))
    if extra:
        fail("save posted unexpected keys %s" % extra, failures)

    immediate = observed.get("immediate") or []
    flat: dict = {}
    for body in immediate:
        if isinstance(body, dict):
            flat.update(body)
    for key, expected in (("p40", 0), ("p41", 1), ("p39", 1)):
        if key not in flat:
            fail("immediate save omitted %s" % key, failures)
        elif not same(flat[key], expected):
            fail("immediate %s: expected %r, got %r" % (key, expected, flat[key]), failures)

    screen = observed.get("screen")
    if not isinstance(screen, dict):
        fail("layout time settings did not POST /api/settings", failures)
        return
    for key, expected in EXPECTED_SCREEN.items():
        if key not in screen:
            fail("screen save omitted %s (expected %r)" % (key, expected), failures)
        elif not same(screen[key], expected):
            fail("screen %s: expected %r, got %r" % (key, expected, screen[key]), failures)

    check_widgets(observed.get("widgets"), failures)

    def find_translation(node: object, key: str) -> str | None:
        if isinstance(node, dict):
            if isinstance(node.get(key), str):
                return node[key]
            for value in node.values():
                found = find_translation(value, key)
                if found is not None:
                    return found
        return None

    for locale in ("da", "de", "en", "es", "fr", "it", "pl", "pt", "sv"):
        try:
            translations = json.loads((ROOT / "spiffs" / "i18n" / f"language_{locale}.json").read_text(encoding="utf-8"))
            message = translations["advanced"]["message"]
            brightness = translations["advanced"]["brightness"]
            curve = brightness["curve"]
        except (OSError, KeyError, json.JSONDecodeError) as exc:
            fail("%s translations could not be checked: %s" % (locale, exc), failures)
            continue
        for key in ("dim_mode_full_hint", "dim_start", "dim_end"):
            if not isinstance(brightness.get(key), str) or not brightness[key]:
                fail("%s brightness translation missing %s" % (locale, key), failures)
        curve_keys = set(re.findall(r"\btr\('([a-z_]+)'", CURVE_JS.read_text(encoding="utf-8")))
        if not curve_keys:
            fail("could not find the adaptive curve translation keys in the editor", failures)
        missing_curve_keys = sorted(
            key for key in curve_keys if not isinstance(curve.get(key), str) or not curve[key]
        )
        if missing_curve_keys:
            fail("%s adaptive curve translations missing %s" %
                 (locale, ", ".join(missing_curve_keys)), failures)
        if "(+3 @ 12:01)" in message.get("token_cgm_glucose", ""):
            fail("%s CGM glucose help still advertises a trend/time suffix" % locale, failures)
        width_label = find_translation(translations, "text_width") or ""
        if "0 = " not in width_label or any(term in width_label.lower() for term in ("no limit", "sans limite", "kein limit", "sin límite", "nessun limite", "bez limitu", "sem limite", "ingen gräns", "ingen grænse")):
            fail("%s width label does not describe automatic sizing" % locale, failures)


def check_widgets(w: dict | None, failures: list[str]) -> None:
    if not isinstance(w, dict):
        fail("harness produced no widget results", failures)
        return

    def expect(label: str, got, expected) -> None:
        if got != expected:
            fail("widgets %s: expected %r, got %r" % (label, expected, got), failures)

    factory = w.get("factory") or {}
    expect("factory clock defaults", factory.get("clock"), {"name": "Clock", "width": 0, "align": 1, "height": 40})
    expect("default canvas template clock defaults", w.get("templateClock"), {"name": "Clock", "width": 0, "align": 1})
    expect("preset widget name omits its token", w.get("templateWeatherName"), "Now")
    expect("factory AMPM defaults", factory.get("ampm"), {"name": "AMPM", "width": 25, "height": 15})
    expect("factory scroll widget is full width", factory.get("scroll"),
           {"name": "Scroll", "enabled": 1, "scroll": True, "width": 128})
    expect("user theme font upload is stored without its prefix", w.get("userThemeUploadPath"),
           "themes/user1/font.jpg")
    expect("theme preview upload is stored as preview.jpg", w.get("themePreviewUploadPath"),
           "themes/bold/preview.jpg")
    expect("nested layout name hides its folder", w.get("layoutDisplayName"), "Default")
    expect("nested layout URL preserves its path separator", w.get("layoutFileUrl"), "/layouts/Default.layout")
    expect("layouts folder accepts saved layouts", w.get("nestedLayoutSaveName"), "layouts/Default.layout")
    expect("layout traversal is rejected", w.get("layoutTraversalRejected"), True)
    expect("nested shipped layout takes precedence over legacy root copy", w.get("layoutNames"),
           ["custom.layout", "layouts/Default.layout"])
    if not 0 < w.get("autoWidth", 0) < 100:
        fail("zero-width widget does not fit its content (width=%r)" % w.get("autoWidth"), failures)
    expect("empty text height uses the selected font", w.get("emptyHeight"), w.get("emptyLineHeight"))
    expect("empty token expands to empty", w.get("emptyTokenSample"), "")
    expect("empty token height uses the selected font", w.get("emptyTokenHeight"), w.get("emptyLineHeight"))
    if w.get("centerShiftAfterSizeChange", 99) > 0.5:
        fail("auto-width font change moved the widget center by %r px" % w.get("centerShiftAfterSizeChange"), failures)
    expect("text widget settings sections", w.get("optionSectionTitles"), ["General", "Font", "Text"])
    expect("font groups remain identifiable without heading-level styling", w.get("fontGroupLabels"),
           ["Text fonts", "Digit themes"])
    expect("font group labels are not section headings", w.get("fontGroupLabelTags"), ["DIV", "DIV"])
    expect("content caption is not a section heading", w.get("contentsLabelTag"), "SPAN")
    expect("enabled setting is in General", w.get("enabledSection"), "General")
    expect("scroll setting is in General", w.get("scrollSection"), "General")
    expect("width setting is in General", w.get("widthSection"), "General")
    expect("font picker is in Font", w.get("fontSectionHasPicker"), True)
    expect("text editor is in Text", w.get("textSection"), "Text")
    expect("default font color mode", w.get("defaultColorMode"), "default")
    expect("custom font color can be enabled", w.get("customColorEnabled"), True)
    expect("none removes the custom font color", w.get("colorModeAfterNone"), False)
    expect("standard widgets have no delete option button", w.get("standardDeleteButton"), False)
    expect("instanced text widgets have a delete option button", w.get("instanceDeleteButton"), True)
    expect("trash drop area is detected", w.get("trashZone"), True)
    expect("dragging a widget to the trash deletes its instance", w.get("deletedByDrop"), 0)
    expect("dragging a standard widget to the trash disables it", w.get("disabledByTrash"), 0)
    expect("weather theme defaults to screen theme", w.get("weatherThemeDefault"), "0")
    expect("weather theme can be overridden", w.get("weatherThemeSelected"), 4)

    initial = w.get("paletteInitial") or []
    expect("palette lists only instances (no add entries)", [i for i in initial if i in ("text", "icon")], [])
    expect("palette keeps the disabled instance", "tw_2" in initial, True)
    expect("palette hides unused slots", "tw_3" in initial, False)

    cards = w.get("fontCards") or []
    expect("font picker has no profile/aux theme or missing user fonts",
           [c for c in cards if "theme" in c.lower() or c in ("User1", "User2")], [])
    expect("font picker keeps the built-in themes", "Nixie" in cards and "Bold" in cards, True)
    expect("active font is highlighted when selected", w.get("activeFontBeforeChange"), "Sans")
    expect("active font highlight follows font changes", w.get("activeFontAfterChange"), "Nixie")

    typed = "[hour24][colon][min]"
    expect("text after typing and a font change", w.get("textAfterFontChange"), typed)
    expect("stored text after typing and a font change", w.get("textStoredAfterFontChange"), typed)
    expect("digit theme selected", w.get("familyAfterFontChange"), 7)  # SPRITE_FIRST + nixie

    added = w.get("added") or {}
    expect("added widget", [added.get("selected"), added.get("enabled"), added.get("instanced"), added.get("name"), added.get("x")],
           ["tw_3", 1, 1, "Text 3", 24])
    if not 50 <= added.get("y", -1) <= 58:
        fail("added widget is not vertically centred (y=%r)" % added.get("y"), failures)
    expect("name clamped to 15 bytes", w.get("renamed"), "Température é")
    expect("palette label is the name", w.get("paletteLabel"), "Température é")
    expect("deleted widget leaves the palette", "tw_3" in (w.get("paletteAfterDelete") or []), False)
    expect("deleted widget is no longer an instance", w.get("deletedInstanced"), 0)

    expect("24h clock tokens", w.get("clock24"), "15:07")
    expect("12h clock tokens", w.get("clock12"), "03:07pm")
    expect("12h clock without leading zero", w.get("clock12NoZero"), "3:07")
    expect("units stripped", w.get("stripped"), "72")
    expect("unknown token kept", w.get("unknown"), "[bogus]")

    wire = w.get("wire") or {}
    expect("wire size", wire.get("size"), wire.get("expected"))
    expect("wire size value", wire.get("size"), 5634)
    expect("weather theme override survives the wire", wire.get("weatherThemeDay"), 4)
    expect("other profile keeps inherited weather theme", wire.get("weatherThemeNight"), 0)
    expect("disabled instance survives the wire", wire.get("tw2"), {"enabled": 0, "instanced": 1, "name": "Off one"})
    expect("custom color override survives the wire", w.get("colorOverrideWire"), {"enabled": True, "color": "#ff4020"})
    expect("version 3 wire still decodes", w.get("v3Decoded"), True)
    expect("version 3 custom color remains custom", w.get("v3ColorOverride"), True)
    expect("version 2 wire still decodes", w.get("v2Decoded"), True)


def macros() -> dict[str, int]:
    text = PWM_H.read_text(encoding="utf-8")
    found = {}
    for name, value in re.findall(r"#define\s+(\w+)\s+(\d+)", text):
        found[name] = int(value)
    return found


def resolve(token: str, defined: dict[str, int]) -> int | None:
    if re.fullmatch(r"-?\d+", token):
        return int(token)
    if token in defined:
        return defined[token]
    return None


def slice_between(text: str, start: str, end: str) -> str:
    i = text.find(start)
    if i < 0:
        raise RuntimeError("missing %s" % start)
    j = text.find(end, i + len(start))
    if j < 0:
        raise RuntimeError("missing %s after %s" % (end, start))
    return text[i:j]


def validation_ranges(text: str, defined: dict[str, int]) -> dict[str, tuple[int, int]]:
    chunk = slice_between(text, "static bool validate_json_params", "esp_err_t screen_get_handler")
    parts = re.split(r'cJSON_GetObjectItem\(root, "(p\d+)"\)', chunk)
    ranges: dict[str, tuple[int, int]] = {}
    for i in range(1, len(parts), 2):
        key, body = parts[i], parts[i + 1]
        m = re.search(
            r"CHECK_(?:DOUBLE_)?RANGE\([^,]+,\s*[^,]+,\s*(-?\d+|[A-Z0-9_]+)\s*,\s*(-?\d+|[A-Z0-9_]+)\)",
            body,
        )
        if not m:
            continue
        lo, hi = resolve(m.group(1), defined), resolve(m.group(2), defined)
        if lo is None or hi is None:
            continue
        ranges[key] = (lo, hi)
    return ranges


def apply_upper_bounds(text: str, defined: dict[str, int]) -> dict[str, int]:
    chunk = slice_between(text, "esp_err_t settings_post_handler", "esp_err_t status_api_handler")
    marks = list(re.finditer(r'cJSON_(?:GetObjectItem|HasObjectItem)\(root, "(p\d+)"\)', chunk))
    bounds: dict[str, int] = {}
    for i, mark in enumerate(marks):
        end = marks[i + 1].start() if i + 1 < len(marks) else len(chunk)
        body = chunk[mark.end() : end]
        caps = []
        for token in re.findall(r"if\s*\([^;]*?<=\s*([A-Za-z0-9_]+)", body):
            value = resolve(token, defined)
            if value is not None:
                caps.append(value)
        if caps:
            # Tightest gate in the apply block is what actually gets stored.
            prev = bounds.get(mark.group(1))
            cap = min(caps)
            bounds[mark.group(1)] = cap if prev is None else min(prev, cap)
    return bounds


def check_firmware(failures: list[str]) -> None:
    text = SETTINGS_C.read_text(encoding="utf-8")
    main_text = MAIN_C.read_text(encoding="utf-8")
    adaptive_text = ADAPTIVE_RUNTIME_C.read_text(encoding="utf-8")
    display_text = DISPLAY_C.read_text(encoding="utf-8")
    index_text = (ROOT / "spiffs" / "index.html").read_text(encoding="utf-8")
    curve_js = CURVE_JS.read_text(encoding="utf-8")
    forms_js = (ROOT / "spiffs" / "js" / "forms.js").read_text(encoding="utf-8")
    components_css = (ROOT / "spiffs" / "css" / "components.css").read_text(encoding="utf-8")
    defined = macros()
    ranges = validation_ranges(text, defined)
    bounds = apply_upper_bounds(text, defined)
    if not ranges:
        fail("parsed no validation ranges from f-settings.c", failures)
        return

    for key in ("p46", "p47", "p55", "p56"):
        if key not in ranges or key not in bounds:
            fail("%s: clock field is missing a validation range or apply gate" % key, failures)

    for key, (lo, hi) in sorted(ranges.items()):
        if key in bounds and bounds[key] < hi:
            fail(
                "%s: validation allows %s-%s but apply only stores <= %s"
                % (key, lo, hi, bounds[key]),
                failures,
            )

    for key, expected in EXPECTED_SAVE.items():
        if key not in ranges or isinstance(expected, str):
            continue
        lo, hi = ranges[key]
        values = expected if isinstance(expected, list) else [expected]
        for value in values:
            if not lo <= float(value) <= hi:
                fail("%s UI value %s outside validation %s-%s" % (key, value, lo, hi), failures)

    curve_validation = slice_between(text, 'if (cJSON_HasObjectItem(root, "p67"))',
                                     '/* p20 lux_sensitivity */')
    for token in (
        "ADAPT_CURVE_POINTS * 2",
        "ADAPT_CURVE_MIN_LUX_X10",
        "ADAPT_CURVE_MAX_LUX_X10",
        "ADAPT_CURVE_MIN_LEVEL",
        "ADAPT_CURVE_MAX_LEVEL",
        "ADAPT_CURVE_MIN_GAP_NUMERATOR",
        "ADAPT_CURVE_MIN_GAP_DENOMINATOR",
    ):
        if token not in curve_validation:
            fail("p67 validation is missing %s" % token, failures)
    if "extended_settings_mask" not in text or 'strcmp(group, "advanced")' not in text:
        fail("p67 is not included in the extended advanced-settings mask", failures)
    if 'cJSON_AddItemToObject(root, "p67", curve_array)' not in text:
        fail("p67 is not returned by the settings API", failures)
    post_text = slice_between(text, "esp_err_t settings_post_handler", "esp_err_t status_api_handler")
    if "adaptive_curve_set(&curve)" not in post_text:
        fail("p67 is not applied by the settings POST handler", failures)
    if '{"adapt_curve", SETTING_TYPE_BLOB' not in main_text:
        fail("adaptive curve is not persisted as an NVS blob", failures)
    adaptive_core_h = (ROOT / "main" / "include" / "f-adaptive-core.h").read_text(encoding="utf-8")
    if "persist_adaptive_defaults" not in main_text or \
       "adaptive_curve_factory_default();" not in main_text or \
       "adaptive_threshold_valid(eeprom_lux_threshold, eeprom_lux_sensitivity)" not in main_text:
        fail("missing or invalid brightness settings do not fall back to stored defaults", failures)
    if "default_from_legacy" in main_text + adaptive_text or "setLegacy" in curve_js + forms_js:
        fail("legacy brightness settings are still converted instead of using defaults", failures)
    if "ADAPT_DEFAULT_THRESHOLD_LUX 2.0f" not in adaptive_core_h or \
       "ADAPT_DEFAULT_SENSITIVITY_LUX 0.5f" not in adaptive_core_h:
        fail("firmware default day threshold and sensitivity do not match the brightness editor", failures)
    for default in (
        "{ lux: 1.2, pct: 2 }",
        "{ lux: 3.2, pct: 44 }",
        "{ lux: 31, pct: 55 }",
        "{ lux: 130, pct: 76 }",
    ):
        if default not in curve_js:
            fail("brightness editor defaults are missing %s" % default, failures)
    if "const DEFAULT_THRESHOLD = 2;" not in curve_js or \
       "const DEFAULT_SENSITIVITY = 0.5;" not in curve_js:
        fail("curve threshold defaults do not match the brightness editor", failures)
    if 'id="lux_threshold"' in index_text or 'id="lux_sensitivity"' in index_text or \
       'class="bc-daynight"' in index_text:
        fail("Day/night switch fields are still rendered next to the adaptive curve", failures)
    if "getThreshold: () =>" not in curve_js or \
       "changedTenth(p, 'p21', thresholds.threshold)" not in forms_js or \
       "changedTenth(p, 'p20', thresholds.sensitivity)" not in forms_js:
        fail("graph day/night handles are not wired to the settings payload", failures)
    if "dimMode.value === '0' && !brightnessCurve.isValid()" not in forms_js:
        fail("curve validation can block saving in non-adaptive dimming modes", failures)
    if 'id="bcChips"' in index_text or re.search(r'id="bc(?:Lux|Pct)\d"', index_text):
        fail("manual point-value fields are still rendered next to the adaptive curve", failures)
    if "bcLux" in curve_js or "bcPct" in curve_js or ".bc-chip" in components_css:
        fail("curve code or styles still depend on removed point-value fields", failures)
    if "or enter exact values" in index_text:
        fail("curve guidance still refers to removed point-value fields", failures)
    if "pointRefs[i].group.addEventListener('keydown'" not in curve_js or \
       "chart.addEventListener('pointerdown', pointerDown)" not in curve_js:
        fail("adaptive reference points are not editable by keyboard and pointer on the curve", failures)
    if "const HISTORY_HEIGHT = 192;" not in curve_js or ".bc-hist{ height:192px; }" not in components_css:
        fail("24-hour brightness history is not three times its original height", failures)
    if "historyCurveSegments" not in curve_js or "bc-history-curve-today" not in curve_js:
        fail("24-hour brightness history is not rendered as a smooth curve", failures)
    if "bc-bar-" in curve_js or "bc-point-label" in curve_js or "bc-value-pill-group" in curve_js:
        fail("brightness history still uses bars or the curve displays point values", failures)
    brightness_target = slice_between(
        adaptive_text,
        "uint8_t brightness_target_percent(void)",
        "void adaptive_tick(float lux)",
    )
    if "if (manufacturer_mode || eeprom_dim_mode == DIM_MODE_FULL)\n        return ADAPT_CURVE_MAX_LEVEL;" not in brightness_target:
        fail("full brightness mode does not target 100 percent", failures)
    if "return time_brightness_percent(font_index);" not in brightness_target:
        fail("time mode does not use the day/night brightness sliders", failures)
    if "eeprom_brightness_LED" in brightness_target or "return curve.pct[0];" not in brightness_target:
        fail("adaptive mode does not use only the curve as its target", failures)
    if "if (!has_applied)" not in adaptive_text or "s_applied_brightness = target;" not in adaptive_text:
        fail("adaptive brightness does not apply its first sensor target immediately", failures)
    time_brightness = slice_between(
        display_text,
        "static bool is_time_of_day_bright(void)",
        "static void handle_als_and_brightness(uint32_t loop_counter)",
    )
    if "if (!time_valid)" not in time_brightness:
        fail("time-of-day dimming does not guard invalid time", failures)
    if "if (!time_valid || !isfinite(lux))" not in adaptive_text:
        fail("hourly ambient-light history does not guard invalid time", failures)
    if "adaptive_tick(sensor_lux)" not in display_text:
        fail("display ALS loop does not feed adaptive brightness/history", failures)
    if "if (!adaptive_mode_active)" not in display_text or "set_led_pwm_brightness(brightness_target_percent())" not in display_text:
        fail("legacy time/full brightness modes do not reapply their target on profile changes", failures)
    if "return brightness_target_percent();" not in PWM_C.read_text(encoding="utf-8"):
        fail("PWM startup does not use the unified brightness target", failures)
    display_title = index_text.find('data-i18n="settings.display.title"')
    regional_title = index_text.find('data-i18n="settings.regional.title"', display_title)
    if display_title < 0 or regional_title < 0:
        fail("display settings panel could not be located in the UI", failures)
    else:
        display_markup = index_text[display_title:regional_title]
        if 'data-i18n="common.advanced"' in display_markup:
            fail("display brightness controls are still inside an Advanced section", failures)
    mode_position = index_text.find('id="dim_mode"')
    if mode_position < 0:
        fail("brightness mode selector is missing from the UI", failures)
    elif index_text.rfind("<details", 0, mode_position) > index_text.rfind("</details>", 0, mode_position):
        fail("brightness mode selector is still inside a details section", failures)
    try:
        dim_time_markup = slice_between(
            index_text,
            '<div id="dimTime"',
            '\n      </div>\n\n      <div class="subtitle" data-i18n="advanced.brightness.led_settings">',
        )
    except RuntimeError as exc:
        fail("time dimming panel could not be located: %s" % exc, failures)
    else:
        if 'id="brightness_LED0"' not in dim_time_markup:
            fail("projection brightness sliders are not inside time dimming", failures)
    try:
        led_settings_markup = slice_between(
            index_text,
            '<div class="subtitle" data-i18n="advanced.brightness.led_settings">',
            '\n    </div>\n\n    <div class="panel collapsible collapsed">',
        )
    except RuntimeError as exc:
        fail("LED settings section could not be located: %s" % exc, failures)
    else:
        if 'id="pwm_frequency"' not in led_settings_markup or 'id="max_power"' not in led_settings_markup:
            fail("LED settings section is missing PWM frequency or max power", failures)
    status_text = slice_between(text, "esp_err_t status_api_handler", "#define FILE_PATH")
    if '"luxhist"' not in status_text or "cJSON_CreateRaw(history_json)" not in status_text:
        fail("status API does not expose optional raw hourly history JSON", failures)
    if "display_get_cached_lux()" not in status_text or "ltr303_get_frixos_lux()" in status_text:
        fail("status API should return cached lux without extra I2C sensor reads", failures)
    if "include_light_sample" not in status_text or "display_get_cached_lux()" not in status_text:
        fail("status API does not provide the compact cached-light polling path", failures)
    if "apiGet('/api/status?light=1')" not in CURVE_JS.read_text(encoding="utf-8"):
        fail("brightness UI does not use the low-cost cached-light status query", failures)


def check_adaptive_core(failures: list[str]) -> None:
    with tempfile.TemporaryDirectory(prefix="frixos-adaptive-test-") as temp_dir:
        executable = Path(temp_dir) / "test_adaptive"
        command = [
            "gcc",
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-I",
            str(ROOT / "main" / "include"),
            str(ADAPTIVE_CORE_C),
            str(ADAPTIVE_TEST_C),
            "-lm",
            "-o",
            str(executable),
        ]
        try:
            compile_result = subprocess.run(
                command,
                cwd=ROOT,
                capture_output=True,
                text=True,
                timeout=60,
                check=False,
            )
        except (OSError, subprocess.TimeoutExpired) as exc:
            fail("adaptive core host compile failed: %s" % exc, failures)
            return
        if compile_result.returncode != 0:
            fail("adaptive core host compile failed\n%s" %
                 (compile_result.stderr or compile_result.stdout), failures)
            return
        run_result = subprocess.run(
            [str(executable)],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=10,
            check=False,
        )
        if run_result.returncode != 0:
            fail("adaptive core host tests failed\n%s" %
                 (run_result.stderr or run_result.stdout), failures)

def main() -> int:
    failures: list[str] = []
    observed = run_harness(failures)
    if observed is not None:
        check_ui(observed, failures)
    try:
        check_firmware(failures)
    except Exception as exc:
        fail("firmware contract: %s" % exc, failures)
    check_adaptive_core(failures)

    if failures:
        print("FAIL %d" % len(failures))
        for item in failures:
            print("- " + item)
        return 1
    print("PASS ui settings (%d save keys, firmware ranges)" % len(EXPECTED_SAVE))
    return 0


if __name__ == "__main__":
    sys.exit(main())
