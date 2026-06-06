/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --no-compress --font c:\source\frixos\managed_components\lvgl__lvgl\scripts\built_in_font\Montserrat-Medium.ttf --range 0-255 --format lvgl --lv-font-name frixos_16 -o c:\source\frixos\main\frixos_16.c
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#ifndef FRIXOS_16
#define FRIXOS_16 1
#endif

#if FRIXOS_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0000 "\u0000" */
    0x0,

    /* U+000D "\r" */
    0x0,

    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xf,

    /* U+0022 "\"" */
    0x99, 0x99,

    /* U+0023 "#" */
    0x11, 0x84, 0x41, 0x13, 0xff, 0x31, 0x8, 0x42,
    0x10, 0x8c, 0xff, 0xc8, 0x82, 0x21, 0x88,

    /* U+0024 "$" */
    0x8, 0x4, 0xf, 0xcd, 0x2c, 0x86, 0x43, 0xa0,
    0xf8, 0x1f, 0x5, 0xc2, 0x61, 0x3c, 0xb3, 0xf0,
    0x20, 0x10,

    /* U+0025 "%" */
    0x70, 0x4d, 0x88, 0x88, 0x88, 0x90, 0x8b, 0xd,
    0xa0, 0x74, 0xe0, 0xd1, 0x9, 0x11, 0x11, 0x11,
    0x12, 0xe,

    /* U+0026 "&" */
    0x3e, 0xc, 0x61, 0x8c, 0x31, 0x83, 0xe0, 0x30,
    0x1f, 0x7, 0x32, 0xc3, 0xd8, 0x31, 0x8f, 0x1f,
    0x30,

    /* U+0027 "'" */
    0xf0,

    /* U+0028 "(" */
    0x36, 0x64, 0xcc, 0xcc, 0xcc, 0xc4, 0x66, 0x30,

    /* U+0029 ")" */
    0xc6, 0x62, 0x33, 0x33, 0x33, 0x32, 0x66, 0xc0,

    /* U+002A "*" */
    0x25, 0x5c, 0xea, 0x90,

    /* U+002B "+" */
    0x18, 0x30, 0x67, 0xf1, 0x83, 0x6, 0x0,

    /* U+002C "," */
    0xfe,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x4, 0x30, 0x82, 0x18, 0x41, 0xc, 0x20, 0x86,
    0x10, 0x43, 0x8, 0x20,

    /* U+0030 "0" */
    0x1c, 0x31, 0x98, 0xd8, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xb1, 0x98, 0xc7, 0xc0,

    /* U+0031 "1" */
    0xf8, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x30,

    /* U+0032 "2" */
    0x7c, 0xc6, 0x3, 0x3, 0x3, 0x7, 0xe, 0x1c,
    0x38, 0x30, 0x60, 0xff,

    /* U+0033 "3" */
    0x7f, 0x1, 0x81, 0x80, 0x80, 0xc0, 0xf0, 0x1c,
    0x3, 0x1, 0x80, 0xd0, 0xcf, 0xc0,

    /* U+0034 "4" */
    0x6, 0x3, 0x80, 0xc0, 0x60, 0x30, 0xc, 0xc6,
    0x33, 0xc, 0xff, 0xc0, 0xc0, 0x30, 0xc,

    /* U+0035 "5" */
    0x3f, 0x30, 0x18, 0xc, 0x6, 0x3, 0xf0, 0xc,
    0x3, 0x1, 0x80, 0xf0, 0xcf, 0xc0,

    /* U+0036 "6" */
    0x1f, 0x18, 0x98, 0x18, 0xc, 0x6, 0xf3, 0x8d,
    0xc3, 0xc1, 0xb0, 0xd8, 0xc3, 0xc0,

    /* U+0037 "7" */
    0xff, 0xe0, 0xb0, 0xc0, 0x40, 0x60, 0x30, 0x10,
    0x18, 0xc, 0x4, 0x6, 0x2, 0x0,

    /* U+0038 "8" */
    0x3e, 0x71, 0xb0, 0x78, 0x36, 0x31, 0xf1, 0x8d,
    0x83, 0xc1, 0xe0, 0xd8, 0xc7, 0xc0,

    /* U+0039 "9" */
    0x3c, 0x31, 0xb0, 0x58, 0x3c, 0x1b, 0x1c, 0xf6,
    0x3, 0x1, 0x81, 0x91, 0x8f, 0x80,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0xf0, 0x3, 0xf8,

    /* U+003C "<" */
    0x0, 0x1c, 0xe7, 0xc, 0x7, 0x3, 0x81,

    /* U+003D "=" */
    0xfe, 0x0, 0x0, 0xf, 0xe0,

    /* U+003E ">" */
    0x1, 0xc0, 0xe0, 0x70, 0x67, 0x38, 0x40,

    /* U+003F "?" */
    0x3e, 0x31, 0xc0, 0x60, 0x30, 0x38, 0x38, 0x38,
    0x18, 0x0, 0x0, 0x3, 0x1, 0x80,

    /* U+0040 "@" */
    0x7, 0xc0, 0x30, 0x60, 0x80, 0x22, 0x3d, 0xa4,
    0xc7, 0x53, 0x6, 0x66, 0xc, 0xcc, 0x19, 0x98,
    0x33, 0x30, 0x65, 0x31, 0xd2, 0x3c, 0xe2, 0x0,
    0x3, 0x4, 0x1, 0xf8, 0x0,

    /* U+0041 "A" */
    0x6, 0x0, 0xe0, 0xf, 0x0, 0x90, 0x19, 0x81,
    0x18, 0x30, 0x83, 0xc, 0x7f, 0xc6, 0x6, 0x40,
    0x6c, 0x3,

    /* U+0042 "B" */
    0xfe, 0x30, 0xcc, 0x1b, 0x6, 0xc1, 0xb0, 0xcf,
    0xf3, 0x6, 0xc0, 0xf0, 0x3c, 0x1f, 0xfc,

    /* U+0043 "C" */
    0xf, 0x86, 0x19, 0x81, 0x70, 0xc, 0x1, 0x80,
    0x30, 0x6, 0x0, 0xe0, 0xc, 0x8, 0xc3, 0x7,
    0xc0,

    /* U+0044 "D" */
    0xff, 0x18, 0x33, 0x3, 0x60, 0x3c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0x1b, 0x6, 0x7f,
    0x80,

    /* U+0045 "E" */
    0xff, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3, 0xfd,
    0x80, 0xc0, 0x60, 0x30, 0x1f, 0xf0,

    /* U+0046 "F" */
    0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0047 "G" */
    0x1f, 0x8c, 0x36, 0x3, 0x0, 0xc0, 0x30, 0xc,
    0xf, 0x3, 0xc0, 0xd8, 0x33, 0xc, 0x7e,

    /* U+0048 "H" */
    0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc0, 0xf0, 0x3f,
    0xff, 0x3, 0xc0, 0xf0, 0x3c, 0xf, 0x3,

    /* U+0049 "I" */
    0xff, 0xff, 0xff,

    /* U+004A "J" */
    0x7e, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x83, 0x6,
    0xf, 0x33, 0xc0,

    /* U+004B "K" */
    0xc0, 0xb0, 0x6c, 0x33, 0x18, 0xcc, 0x36, 0xf,
    0xc3, 0x98, 0xc7, 0x30, 0xcc, 0x1b, 0x3,

    /* U+004C "L" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xff,

    /* U+004D "M" */
    0xc0, 0x3c, 0x3, 0xe0, 0x7e, 0x7, 0xd0, 0xbd,
    0x9b, 0xc9, 0x3c, 0xf3, 0xc6, 0x3c, 0x63, 0xc0,
    0x3c, 0x3,

    /* U+004E "N" */
    0xc0, 0xf8, 0x3e, 0xf, 0xc3, 0xd8, 0xf3, 0x3c,
    0xcf, 0x1b, 0xc3, 0xf0, 0x7c, 0x1f, 0x3,

    /* U+004F "O" */
    0xf, 0x83, 0xc, 0x60, 0x6c, 0x6, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x76, 0x6, 0x30,
    0xc1, 0xf8,

    /* U+0050 "P" */
    0xfe, 0x61, 0xb0, 0x78, 0x3c, 0x1e, 0xf, 0xd,
    0xfc, 0xc0, 0x60, 0x30, 0x18, 0x0,

    /* U+0051 "Q" */
    0xf, 0x83, 0xc, 0x60, 0x64, 0x6, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x76, 0x6, 0x30,
    0xc1, 0xf8, 0x3, 0x10, 0xf,

    /* U+0052 "R" */
    0xfe, 0x61, 0xb0, 0x78, 0x3c, 0x1e, 0xf, 0xd,
    0xfc, 0xc6, 0x61, 0x30, 0xd8, 0x30,

    /* U+0053 "S" */
    0x3f, 0x30, 0xb0, 0x18, 0xe, 0x3, 0xf0, 0xfc,
    0x7, 0x1, 0x80, 0xf0, 0xcf, 0xc0,

    /* U+0054 "T" */
    0xff, 0x86, 0x3, 0x1, 0x80, 0xc0, 0x60, 0x30,
    0x18, 0xc, 0x6, 0x3, 0x1, 0x80,

    /* U+0055 "U" */
    0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xd0, 0x26, 0x18, 0xfc,

    /* U+0056 "V" */
    0xc0, 0x28, 0xd, 0x81, 0x30, 0x63, 0xc, 0x61,
    0x4, 0x60, 0xc8, 0x1b, 0x1, 0xe0, 0x38, 0x3,
    0x0,

    /* U+0057 "W" */
    0x81, 0x81, 0x60, 0xc1, 0xb0, 0x70, 0xc8, 0x78,
    0x46, 0x24, 0x23, 0x13, 0x30, 0x99, 0x90, 0x48,
    0x48, 0x34, 0x2c, 0x1e, 0x1e, 0x6, 0x6, 0x3,
    0x3, 0x0,

    /* U+0058 "X" */
    0x60, 0xcc, 0x10, 0xc6, 0xd, 0x80, 0xe0, 0x1c,
    0x3, 0x80, 0xd0, 0x13, 0x6, 0x31, 0x83, 0x20,
    0x60,

    /* U+0059 "Y" */
    0xc0, 0x70, 0x36, 0x18, 0x84, 0x33, 0x6, 0x81,
    0xe0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30,

    /* U+005A "Z" */
    0xff, 0x81, 0xc0, 0xc0, 0xc0, 0xc0, 0x60, 0x60,
    0x60, 0x30, 0x30, 0x30, 0x1f, 0xf0,

    /* U+005B "[" */
    0xfc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xf0,

    /* U+005C "\\" */
    0x40, 0x81, 0x81, 0x2, 0x6, 0x4, 0x8, 0x18,
    0x10, 0x20, 0x60, 0x40, 0x81, 0x83,

    /* U+005D "]" */
    0xf3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xf0,

    /* U+005E "^" */
    0x18, 0x70, 0xa1, 0x24, 0x48, 0xf0, 0x80,

    /* U+005F "_" */
    0xff,

    /* U+0060 "`" */
    0x42,

    /* U+0061 "a" */
    0x7c, 0x46, 0x3, 0x3, 0x7f, 0xc3, 0xc3, 0xc7,
    0x7b,

    /* U+0062 "b" */
    0xc0, 0x60, 0x30, 0x1b, 0xce, 0x36, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf8, 0xdb, 0xc0,

    /* U+0063 "c" */
    0x3e, 0x63, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x63,
    0x1e,

    /* U+0064 "d" */
    0x1, 0x80, 0xc0, 0x67, 0xb6, 0x3e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xd8, 0xe7, 0xb0,

    /* U+0065 "e" */
    0x3c, 0x66, 0xc3, 0xc1, 0xff, 0xc0, 0xc0, 0x62,
    0x3e,

    /* U+0066 "f" */
    0x3d, 0x86, 0x3e, 0x61, 0x86, 0x18, 0x61, 0x86,
    0x18,

    /* U+0067 "g" */
    0x3d, 0xb1, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x6,
    0xc7, 0x3d, 0x80, 0xd0, 0xcf, 0xc0,

    /* U+0068 "h" */
    0xc0, 0xc0, 0xc0, 0xde, 0xe6, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3,

    /* U+0069 "i" */
    0xf3, 0xff, 0xff,

    /* U+006A "j" */
    0x18, 0xc0, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8f, 0xc0,

    /* U+006B "k" */
    0xc0, 0x60, 0x30, 0x18, 0x6c, 0x66, 0x63, 0x61,
    0xf8, 0xec, 0x63, 0x30, 0xd8, 0x20,

    /* U+006C "l" */
    0xff, 0xff, 0xff,

    /* U+006D "m" */
    0xde, 0xf3, 0x9e, 0x6c, 0x30, 0xf0, 0xc3, 0xc3,
    0xf, 0xc, 0x3c, 0x30, 0xf0, 0xc3, 0xc3, 0xc,

    /* U+006E "n" */
    0xde, 0xe6, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3,

    /* U+006F "o" */
    0x3e, 0x31, 0xb0, 0x78, 0x3c, 0x1e, 0xf, 0x6,
    0xc6, 0x3e, 0x0,

    /* U+0070 "p" */
    0xde, 0x71, 0xb0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0xc6, 0xde, 0x60, 0x30, 0x18, 0x0,

    /* U+0071 "q" */
    0x3d, 0xb1, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x6,
    0xc7, 0x3d, 0x80, 0xc0, 0x60, 0x30,

    /* U+0072 "r" */
    0xdf, 0x31, 0x8c, 0x63, 0x18, 0xc0,

    /* U+0073 "s" */
    0x7d, 0x8b, 0x7, 0x7, 0xc3, 0xc1, 0xc3, 0xfc,

    /* U+0074 "t" */
    0x61, 0x8f, 0x98, 0x61, 0x86, 0x18, 0x61, 0x83,
    0xc0,

    /* U+0075 "u" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x67,
    0x7b,

    /* U+0076 "v" */
    0xc1, 0xa0, 0x98, 0xcc, 0x62, 0x21, 0xb0, 0x50,
    0x38, 0x18, 0x0,

    /* U+0077 "w" */
    0xc3, 0x5, 0xc, 0x34, 0x38, 0x99, 0xa2, 0x24,
    0x98, 0x93, 0x43, 0xc5, 0x6, 0x1c, 0x18, 0x60,

    /* U+0078 "x" */
    0x43, 0x31, 0xd, 0x83, 0x81, 0x80, 0xe0, 0xd8,
    0xc6, 0x43, 0x0,

    /* U+0079 "y" */
    0xc1, 0xa0, 0x98, 0xcc, 0x42, 0x21, 0xb0, 0x50,
    0x38, 0x8, 0xc, 0x24, 0x1e, 0x0,

    /* U+007A "z" */
    0xfe, 0xc, 0x30, 0xc1, 0x6, 0x18, 0x60, 0xfe,

    /* U+007B "{" */
    0x1c, 0xc3, 0xc, 0x30, 0xc3, 0x38, 0x30, 0xc3,
    0xc, 0x30, 0xc1, 0xc0,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xfc,

    /* U+007D "}" */
    0xe1, 0x8c, 0x63, 0x18, 0xc3, 0x31, 0x8c, 0x63,
    0x1b, 0x80,

    /* U+007E "~" */
    0x63, 0x26, 0x30,

    /* U+00A0 " " */
    0x0,

    /* U+00A1 "¡" */
    0xf0, 0xff, 0xff,

    /* U+00A2 "¢" */
    0x8, 0x8, 0x3e, 0x7f, 0xe8, 0xc8, 0xc8, 0xc8,
    0xe8, 0x7f, 0x3e, 0x8, 0x8,

    /* U+00A3 "£" */
    0xf, 0x8c, 0x4c, 0x6, 0x3, 0x1, 0x83, 0xfc,
    0x60, 0x30, 0x18, 0xc, 0x1f, 0xf0,

    /* U+00A4 "¤" */
    0x40, 0xd, 0xe8, 0xc2, 0x18, 0x66, 0x4, 0xc0,
    0x98, 0x11, 0x86, 0x30, 0x8d, 0xf9, 0x1, 0x0,

    /* U+00A5 "¥" */
    0xc0, 0x36, 0x6, 0x20, 0x43, 0xc, 0x19, 0x80,
    0x90, 0xf, 0x3, 0xfc, 0x6, 0x3, 0xfc, 0x6,
    0x0, 0x60,

    /* U+00A6 "¦" */
    0xff, 0xf0, 0x3f, 0xfc,

    /* U+00A7 "§" */
    0x7f, 0x87, 0x7, 0x7, 0xd9, 0xf1, 0xf3, 0x7c,
    0x1c, 0x1c, 0x3f, 0xc0,

    /* U+00A8 "¨" */
    0xd8,

    /* U+00A9 "©" */
    0x1f, 0x6, 0x31, 0x1, 0x67, 0xb9, 0x93, 0x20,
    0x64, 0xc, 0xc9, 0xcf, 0x68, 0x8, 0xc6, 0xf,
    0x80,

    /* U+00AA "ª" */
    0x70, 0x7f, 0x1f, 0x80,

    /* U+00AB "«" */
    0x24, 0x93, 0x66, 0xc4, 0x84, 0x80,

    /* U+00AC "¬" */
    0xff, 0x3, 0x3, 0x3,

    /* U+00AD "­" */
    0xf0,

    /* U+00AE "®" */
    0x1f, 0x6, 0x31, 0x1, 0x6f, 0xb9, 0xb, 0x21,
    0x67, 0xcc, 0x89, 0xd0, 0xe8, 0x8, 0xc6, 0xf,
    0x80,

    /* U+00AF "¯" */
    0xf8,

    /* U+00B0 "°" */
    0x76, 0xe3, 0x1d, 0xb8,

    /* U+00B1 "±" */
    0x18, 0x30, 0x67, 0xf1, 0x83, 0x6, 0x0, 0x1,
    0xfc,

    /* U+00B2 "²" */
    0xf4, 0x42, 0x22, 0x23, 0xe0,

    /* U+00B3 "³" */
    0x7c, 0x21, 0xe, 0x4, 0x17, 0x80,

    /* U+00B4 "´" */
    0x6c,

    /* U+00B5 "µ" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7,
    0xff, 0xc0, 0xc0, 0xc0,

    /* U+00B6 "¶" */
    0x3f, 0xbe, 0xff, 0x6f, 0xb7, 0xd9, 0xec, 0x36,
    0x1b, 0xd, 0x86, 0xc3, 0x61, 0xb0, 0xd8, 0x6c,

    /* U+00B7 "·" */
    0xf0,

    /* U+00B8 "¸" */
    0x44, 0xe0,

    /* U+00B9 "¹" */
    0xe1, 0x8, 0x42, 0x13, 0xe0,

    /* U+00BA "º" */
    0x7a, 0x38, 0x61, 0x78,

    /* U+00BB "»" */
    0x91, 0x26, 0xdb, 0x4a, 0x40,

    /* U+00BC "¼" */
    0xe0, 0x30, 0x40, 0x40, 0x81, 0x1, 0x4, 0x2,
    0x18, 0x4, 0x21, 0x3e, 0x84, 0x3, 0x18, 0x4,
    0x24, 0x10, 0xfc, 0x60, 0x10, 0x80, 0x20,

    /* U+00BD "½" */
    0xe0, 0x20, 0x81, 0x2, 0x4, 0x8, 0x20, 0x21,
    0x80, 0x85, 0xef, 0xa0, 0x41, 0x81, 0x4, 0x8,
    0x20, 0x40, 0x82, 0x4, 0x1f,

    /* U+00BE "¾" */
    0x7c, 0x18, 0x8, 0x10, 0x10, 0x20, 0x38, 0x60,
    0x4, 0x40, 0x4, 0x84, 0x79, 0x88, 0x1, 0x18,
    0x2, 0x12, 0x4, 0x3f, 0x4, 0x2, 0x8, 0x2,

    /* U+00BF "¿" */
    0x18, 0x18, 0x0, 0x0, 0x18, 0x30, 0x60, 0xc0,
    0xc0, 0xe3, 0x7e,

    /* U+00C0 "À" */
    0x18, 0x0, 0x40, 0x2, 0x0, 0x60, 0xe, 0x0,
    0xf0, 0x9, 0x1, 0x98, 0x11, 0x83, 0x8, 0x30,
    0xc7, 0xfc, 0x60, 0x64, 0x6, 0xc0, 0x30,

    /* U+00C1 "Á" */
    0x3, 0x0, 0x20, 0x4, 0x0, 0x60, 0xe, 0x0,
    0xf0, 0x9, 0x1, 0x98, 0x11, 0x83, 0x8, 0x30,
    0xc7, 0xfc, 0x60, 0x64, 0x6, 0xc0, 0x30,

    /* U+00C2 "Â" */
    0x6, 0x0, 0xb0, 0x11, 0x80, 0x60, 0xe, 0x0,
    0xf0, 0x9, 0x1, 0x98, 0x11, 0x83, 0x8, 0x30,
    0xc7, 0xfc, 0x60, 0x64, 0x6, 0xc0, 0x30,

    /* U+00C3 "Ã" */
    0x1d, 0x1, 0x30, 0x0, 0x0, 0x60, 0xe, 0x0,
    0xb0, 0x1b, 0x1, 0x90, 0x11, 0x83, 0x8, 0x20,
    0xc7, 0xfc, 0x40, 0x6c, 0x6, 0xc0, 0x20,

    /* U+00C4 "Ä" */
    0xd, 0x80, 0x0, 0x0, 0x80, 0xe, 0x0, 0x50,
    0x6, 0xc0, 0x22, 0x3, 0x18, 0x18, 0xc1, 0x83,
    0xf, 0xf8, 0x40, 0x46, 0x3, 0x20, 0x8,

    /* U+00C5 "Å" */
    0x6, 0x0, 0x90, 0x9, 0x0, 0x60, 0x0, 0x0,
    0x60, 0x6, 0x0, 0xf0, 0x9, 0x1, 0x98, 0x10,
    0x83, 0xc, 0x30, 0xc7, 0xfe, 0x60, 0x64, 0x2,
    0xc0, 0x30,

    /* U+00C6 "Æ" */
    0x1, 0xff, 0x2, 0xc0, 0x2, 0xc0, 0x4, 0xc0,
    0xc, 0xc0, 0xc, 0xc0, 0x18, 0xfe, 0x18, 0xc0,
    0x3f, 0xc0, 0x20, 0xc0, 0x60, 0xc0, 0x40, 0xff,

    /* U+00C7 "Ç" */
    0x1f, 0x8c, 0x36, 0x3, 0x0, 0xc0, 0x30, 0xc,
    0x3, 0x0, 0xc0, 0x18, 0x3, 0xc, 0x7e, 0x4,
    0x0, 0x80, 0xe0,

    /* U+00C8 "È" */
    0x30, 0xc, 0x3, 0x1f, 0xec, 0x6, 0x3, 0x1,
    0x80, 0xc0, 0x7f, 0xb0, 0x18, 0xc, 0x6, 0x3,
    0xfe,

    /* U+00C9 "É" */
    0x6, 0x6, 0x6, 0x1f, 0xec, 0x6, 0x3, 0x1,
    0x80, 0xc0, 0x7f, 0xb0, 0x18, 0xc, 0x6, 0x3,
    0xfe,

    /* U+00CA "Ê" */
    0x1c, 0x1a, 0x18, 0x9f, 0xec, 0x6, 0x3, 0x1,
    0x80, 0xc0, 0x7f, 0xb0, 0x18, 0xc, 0x6, 0x3,
    0xfe,

    /* U+00CB "Ë" */
    0x2c, 0x0, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xff,

    /* U+00CC "Ì" */
    0xc2, 0x13, 0x33, 0x33, 0x33, 0x33, 0x33, 0x30,

    /* U+00CD "Í" */
    0x34, 0x8c, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xc0,

    /* U+00CE "Î" */
    0x31, 0x28, 0x4c, 0x30, 0xc3, 0xc, 0x30, 0xc3,
    0xc, 0x30, 0xc3, 0x0,

    /* U+00CF "Ï" */
    0xd8, 0x18, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6,
    0x30,

    /* U+00D0 "Ð" */
    0x3f, 0xc1, 0x83, 0xc, 0xc, 0x60, 0x33, 0x1,
    0x98, 0xf, 0xf8, 0x66, 0x3, 0x30, 0x19, 0x81,
    0x8c, 0x18, 0x7f, 0x80,

    /* U+00D1 "Ñ" */
    0x1a, 0x9, 0x80, 0x3, 0x3, 0xe0, 0xf8, 0x3f,
    0xf, 0x63, 0xcc, 0xf3, 0x3c, 0x6f, 0xf, 0xc1,
    0xf0, 0x7c, 0xc,

    /* U+00D2 "Ò" */
    0xc, 0x0, 0x60, 0x0, 0x0, 0xf8, 0x30, 0xc6,
    0x6, 0xc0, 0x6c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x3c, 0x7, 0x60, 0x63, 0xc, 0x1f, 0x80,

    /* U+00D3 "Ó" */
    0x3, 0x0, 0x60, 0x0, 0x0, 0xf8, 0x30, 0xc6,
    0x6, 0xc0, 0x6c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x3c, 0x7, 0x60, 0x63, 0xc, 0x1f, 0x80,

    /* U+00D4 "Ô" */
    0x6, 0x0, 0x90, 0x0, 0x0, 0xf8, 0x30, 0xc6,
    0x6, 0xc0, 0x6c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x3c, 0x7, 0x60, 0x63, 0xc, 0x1f, 0x80,

    /* U+00D5 "Õ" */
    0xd, 0x1, 0x30, 0x0, 0x0, 0xf8, 0x30, 0xc6,
    0x6, 0xc0, 0x6c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x3c, 0x7, 0x60, 0x63, 0xc, 0x1f, 0x80,

    /* U+00D6 "Ö" */
    0x1b, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x30, 0xc6,
    0x6, 0xc0, 0x6c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x3c, 0x7, 0x60, 0x63, 0xc, 0x1f, 0x80,

    /* U+00D7 "×" */
    0x8b, 0x67, 0x1c, 0xda, 0x20,

    /* U+00D8 "Ø" */
    0x0, 0x20, 0xfc, 0x30, 0xc6, 0xe, 0xc1, 0x6c,
    0x33, 0xc2, 0x3c, 0x43, 0xcc, 0x3c, 0x87, 0x70,
    0x63, 0xc, 0x3f, 0x86, 0x0,

    /* U+00D9 "Ù" */
    0x18, 0x3, 0x0, 0x3, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0x40,
    0x98, 0x63, 0xf0,

    /* U+00DA "Ú" */
    0x6, 0x3, 0x0, 0x3, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0x40,
    0x98, 0x63, 0xf0,

    /* U+00DB "Û" */
    0xc, 0x4, 0x80, 0x3, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0x40,
    0x98, 0x63, 0xf0,

    /* U+00DC "Ü" */
    0x36, 0x0, 0x0, 0x3, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0x40,
    0x98, 0x63, 0xf0,

    /* U+00DD "Ý" */
    0x3, 0x1, 0x0, 0x83, 0x1, 0xc0, 0xd8, 0x62,
    0x10, 0xcc, 0x1a, 0x7, 0x80, 0xc0, 0x30, 0xc,
    0x3, 0x0, 0xc0,

    /* U+00DE "Þ" */
    0xc0, 0x60, 0x3f, 0x98, 0x6c, 0x1e, 0xf, 0x7,
    0x83, 0xc3, 0x7f, 0x30, 0x18, 0x0,

    /* U+00DF "ß" */
    0x3c, 0x33, 0xb0, 0xd8, 0x6c, 0x66, 0x73, 0xd,
    0x83, 0xc1, 0xe0, 0xf0, 0xdb, 0xc0,

    /* U+00E0 "à" */
    0x30, 0x18, 0x0, 0x7c, 0x46, 0x3, 0x3, 0x7f,
    0xc3, 0xc3, 0xc7, 0x7b,

    /* U+00E1 "á" */
    0x6, 0x18, 0x0, 0x7c, 0x46, 0x3, 0x3, 0x7f,
    0xc3, 0xc3, 0xc7, 0x7b,

    /* U+00E2 "â" */
    0x18, 0x26, 0x0, 0x7c, 0x46, 0x3, 0x3, 0x7f,
    0xc3, 0xc3, 0xc7, 0x7b,

    /* U+00E3 "ã" */
    0x32, 0x2c, 0x0, 0x7c, 0x46, 0x3, 0x3, 0x7f,
    0xc3, 0xc3, 0xc7, 0x7b,

    /* U+00E4 "ä" */
    0x2c, 0x0, 0x0, 0x7c, 0x46, 0x3, 0x3, 0x7f,
    0xc3, 0xc3, 0xc7, 0x7b,

    /* U+00E5 "å" */
    0x18, 0x24, 0x24, 0x18, 0x0, 0x7c, 0x46, 0x3,
    0x3, 0x7f, 0xc3, 0xc3, 0xc7, 0x7b,

    /* U+00E6 "æ" */
    0x7c, 0xf8, 0x8f, 0x18, 0xc, 0x18, 0x18, 0x37,
    0xff, 0xf8, 0x60, 0x30, 0xc0, 0x63, 0xc2, 0x7c,
    0xfc,

    /* U+00E7 "ç" */
    0x3e, 0x63, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x63,
    0x3e, 0x8, 0x4, 0x1c,

    /* U+00E8 "è" */
    0x30, 0x18, 0x0, 0x3c, 0x66, 0xc3, 0xc1, 0xff,
    0xc0, 0xc0, 0x62, 0x3e,

    /* U+00E9 "é" */
    0xc, 0x18, 0x0, 0x3c, 0x66, 0xc3, 0xc1, 0xff,
    0xc0, 0xc0, 0x62, 0x3e,

    /* U+00EA "ê" */
    0x18, 0x24, 0x0, 0x3c, 0x66, 0xc3, 0xc1, 0xff,
    0xc0, 0xc0, 0x62, 0x3e,

    /* U+00EB "ë" */
    0x36, 0x0, 0x0, 0x3c, 0x62, 0xc1, 0xc1, 0xff,
    0xc0, 0xc0, 0x63, 0x3e,

    /* U+00EC "ì" */
    0x63, 0x3, 0x33, 0x33, 0x33, 0x33,

    /* U+00ED "í" */
    0x2c, 0xc, 0xcc, 0xcc, 0xcc, 0xcc,

    /* U+00EE "î" */
    0x31, 0x20, 0xc, 0x30, 0xc3, 0xc, 0x30, 0xc3,
    0xc,

    /* U+00EF "ï" */
    0xf0, 0x6, 0x66, 0x66, 0x66, 0x66,

    /* U+00F0 "ð" */
    0x7d, 0x7, 0x6, 0xc4, 0x30, 0x19, 0xed, 0x8f,
    0x87, 0xc1, 0xe1, 0x98, 0xc7, 0x80,

    /* U+00F1 "ñ" */
    0x34, 0x2c, 0x0, 0xde, 0xe6, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3,

    /* U+00F2 "ò" */
    0x30, 0x4, 0x0, 0x7, 0xc6, 0x36, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xd8, 0xc7, 0xc0,

    /* U+00F3 "ó" */
    0x4, 0x4, 0x0, 0x7, 0xc6, 0x36, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xd8, 0xc7, 0xc0,

    /* U+00F4 "ô" */
    0x1c, 0x1b, 0x0, 0x7, 0xc6, 0x36, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xd8, 0xc7, 0xc0,

    /* U+00F5 "õ" */
    0x1a, 0x16, 0x0, 0x7, 0xc6, 0x36, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xd8, 0xc7, 0xc0,

    /* U+00F6 "ö" */
    0x36, 0x0, 0x0, 0x7, 0xc6, 0x36, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xd8, 0xc7, 0xc0,

    /* U+00F7 "÷" */
    0x18, 0x30, 0x0, 0xf, 0xe0, 0x0, 0xc, 0x18,

    /* U+00F8 "ø" */
    0x1, 0x1f, 0x98, 0xd8, 0xbc, 0xde, 0x4f, 0x67,
    0xa3, 0x63, 0x3f, 0x10, 0x0,

    /* U+00F9 "ù" */
    0x30, 0x18, 0x0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x67, 0x7b,

    /* U+00FA "ú" */
    0xc, 0x18, 0x0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x67, 0x7b,

    /* U+00FB "û" */
    0x18, 0x24, 0x0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x67, 0x7b,

    /* U+00FC "ü" */
    0x3c, 0x0, 0x0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x67, 0x7b,

    /* U+00FD "ý" */
    0x4, 0x4, 0x0, 0x18, 0x34, 0x13, 0x19, 0x88,
    0x44, 0x36, 0xa, 0x7, 0x1, 0x1, 0x84, 0x83,
    0xc0,

    /* U+00FE "þ" */
    0xc0, 0x60, 0x30, 0x1b, 0xce, 0x36, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf8, 0xdb, 0xcc, 0x6, 0x3,
    0x0,

    /* U+00FF "ÿ" */
    0x36, 0x0, 0x0, 0x18, 0x14, 0x1b, 0x8, 0x8c,
    0x64, 0x16, 0xb, 0x7, 0x1, 0x80, 0x84, 0xc3,
    0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 0, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 69, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 69, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 69, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 6, .adv_w = 100, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 8, .adv_w = 180, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 23, .adv_w = 159, .box_w = 9, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 41, .adv_w = 216, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 176, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 76, .adv_w = 54, .box_w = 1, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 77, .adv_w = 86, .box_w = 4, .box_h = 15, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 85, .adv_w = 87, .box_w = 4, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 93, .adv_w = 102, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 97, .adv_w = 149, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 104, .adv_w = 58, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 105, .adv_w = 98, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 106, .adv_w = 58, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 90, .box_w = 6, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 119, .adv_w = 171, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 95, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 141, .adv_w = 147, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 153, .adv_w = 146, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 167, .adv_w = 171, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 147, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 196, .adv_w = 158, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 153, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 224, .adv_w = 165, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 238, .adv_w = 158, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 252, .adv_w = 58, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 255, .adv_w = 58, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 258, .adv_w = 149, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 265, .adv_w = 149, .box_w = 7, .box_h = 5, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 270, .adv_w = 149, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 277, .adv_w = 147, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 291, .adv_w = 265, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 320, .adv_w = 187, .box_w = 12, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 338, .adv_w = 194, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 353, .adv_w = 185, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 370, .adv_w = 211, .box_w = 11, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 387, .adv_w = 172, .box_w = 9, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 401, .adv_w = 163, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 413, .adv_w = 198, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 428, .adv_w = 208, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 443, .adv_w = 79, .box_w = 2, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 446, .adv_w = 131, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 457, .adv_w = 184, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 472, .adv_w = 152, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 484, .adv_w = 244, .box_w = 12, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 502, .adv_w = 208, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 517, .adv_w = 215, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 535, .adv_w = 185, .box_w = 9, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 549, .adv_w = 215, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 570, .adv_w = 186, .box_w = 9, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 584, .adv_w = 159, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 598, .adv_w = 150, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 612, .adv_w = 202, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 627, .adv_w = 182, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 644, .adv_w = 288, .box_w = 17, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 670, .adv_w = 172, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 687, .adv_w = 166, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 702, .adv_w = 168, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 716, .adv_w = 85, .box_w = 4, .box_h = 15, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 724, .adv_w = 90, .box_w = 7, .box_h = 16, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 738, .adv_w = 85, .box_w = 4, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 746, .adv_w = 149, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 753, .adv_w = 128, .box_w = 8, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 754, .adv_w = 154, .box_w = 4, .box_h = 2, .ofs_x = 2, .ofs_y = 10},
    {.bitmap_index = 755, .adv_w = 153, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 764, .adv_w = 175, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 778, .adv_w = 146, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 787, .adv_w = 175, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 801, .adv_w = 157, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 810, .adv_w = 90, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 819, .adv_w = 177, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 833, .adv_w = 174, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 845, .adv_w = 71, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 848, .adv_w = 73, .box_w = 5, .box_h = 15, .ofs_x = -2, .ofs_y = -3},
    {.bitmap_index = 858, .adv_w = 158, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 872, .adv_w = 71, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 875, .adv_w = 271, .box_w = 14, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 891, .adv_w = 174, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 900, .adv_w = 163, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 911, .adv_w = 175, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 925, .adv_w = 175, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 939, .adv_w = 105, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 945, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 953, .adv_w = 106, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 962, .adv_w = 173, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 971, .adv_w = 143, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 982, .adv_w = 230, .box_w = 14, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 998, .adv_w = 141, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1009, .adv_w = 143, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1023, .adv_w = 133, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1031, .adv_w = 90, .box_w = 6, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1043, .adv_w = 77, .box_w = 2, .box_h = 15, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 1047, .adv_w = 90, .box_w = 5, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1057, .adv_w = 149, .box_w = 7, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 1060, .adv_w = 69, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1061, .adv_w = 69, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1064, .adv_w = 146, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1077, .adv_w = 165, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1091, .adv_w = 179, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1107, .adv_w = 181, .box_w = 12, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1125, .adv_w = 77, .box_w = 2, .box_h = 15, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 1129, .adv_w = 128, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1141, .adv_w = 154, .box_w = 5, .box_h = 1, .ofs_x = 2, .ofs_y = 11},
    {.bitmap_index = 1142, .adv_w = 205, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1159, .adv_w = 103, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 1163, .adv_w = 129, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 1169, .adv_w = 149, .box_w = 8, .box_h = 4, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 1173, .adv_w = 98, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 1174, .adv_w = 205, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1191, .adv_w = 154, .box_w = 5, .box_h = 1, .ofs_x = 2, .ofs_y = 11},
    {.bitmap_index = 1192, .adv_w = 107, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 1196, .adv_w = 149, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1205, .adv_w = 110, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 1210, .adv_w = 110, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 1216, .adv_w = 154, .box_w = 4, .box_h = 2, .ofs_x = 4, .ofs_y = 10},
    {.bitmap_index = 1217, .adv_w = 175, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1229, .adv_w = 166, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1245, .adv_w = 68, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 1246, .adv_w = 154, .box_w = 3, .box_h = 4, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1248, .adv_w = 110, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 1253, .adv_w = 107, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 1257, .adv_w = 129, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 1262, .adv_w = 265, .box_w = 15, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1285, .adv_w = 265, .box_w = 14, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1306, .adv_w = 265, .box_w = 16, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1330, .adv_w = 147, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1341, .adv_w = 187, .box_w = 12, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1364, .adv_w = 187, .box_w = 12, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1387, .adv_w = 187, .box_w = 12, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1410, .adv_w = 187, .box_w = 12, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1433, .adv_w = 187, .box_w = 13, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1456, .adv_w = 187, .box_w = 12, .box_h = 17, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1482, .adv_w = 268, .box_w = 16, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1506, .adv_w = 185, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1525, .adv_w = 172, .box_w = 9, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1542, .adv_w = 172, .box_w = 9, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1559, .adv_w = 172, .box_w = 9, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1576, .adv_w = 172, .box_w = 8, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1590, .adv_w = 79, .box_w = 4, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1598, .adv_w = 79, .box_w = 4, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1606, .adv_w = 79, .box_w = 6, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1618, .adv_w = 79, .box_w = 5, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1627, .adv_w = 214, .box_w = 13, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1647, .adv_w = 208, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1666, .adv_w = 215, .box_w = 12, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1689, .adv_w = 215, .box_w = 12, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1712, .adv_w = 215, .box_w = 12, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1735, .adv_w = 215, .box_w = 12, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1758, .adv_w = 215, .box_w = 12, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1781, .adv_w = 149, .box_w = 6, .box_h = 6, .ofs_x = 2, .ofs_y = 3},
    {.bitmap_index = 1786, .adv_w = 215, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1807, .adv_w = 202, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1826, .adv_w = 202, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1845, .adv_w = 202, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1864, .adv_w = 202, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1883, .adv_w = 166, .box_w = 10, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1902, .adv_w = 185, .box_w = 9, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1916, .adv_w = 173, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1930, .adv_w = 153, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1942, .adv_w = 153, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1954, .adv_w = 153, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1966, .adv_w = 153, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1978, .adv_w = 153, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1990, .adv_w = 153, .box_w = 8, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2004, .adv_w = 253, .box_w = 15, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2021, .adv_w = 146, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 2033, .adv_w = 157, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2045, .adv_w = 157, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2057, .adv_w = 157, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2069, .adv_w = 157, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2081, .adv_w = 71, .box_w = 4, .box_h = 12, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 2087, .adv_w = 71, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2093, .adv_w = 71, .box_w = 6, .box_h = 12, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 2102, .adv_w = 71, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2108, .adv_w = 164, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2122, .adv_w = 174, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2134, .adv_w = 163, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2148, .adv_w = 163, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2162, .adv_w = 163, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2176, .adv_w = 163, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2190, .adv_w = 163, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2204, .adv_w = 149, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 2212, .adv_w = 163, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 2225, .adv_w = 173, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2237, .adv_w = 173, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2249, .adv_w = 173, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2261, .adv_w = 173, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2273, .adv_w = 143, .box_w = 9, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 2290, .adv_w = 175, .box_w = 9, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 2307, .adv_w = 143, .box_w = 9, .box_h = 15, .ofs_x = 0, .ofs_y = -3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xd
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 0, .range_length = 14, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 3,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 160, .range_length = 96, .glyph_id_start = 98,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 0, 1, 2, 0, 3,
    4, 5, 2, 6, 7, 8, 9, 10,
    9, 10, 11, 12, 0, 13, 14, 15,
    16, 17, 18, 19, 12, 20, 20, 0,
    0, 0, 21, 22, 23, 24, 25, 22,
    26, 27, 28, 29, 29, 30, 31, 32,
    29, 29, 22, 33, 34, 35, 3, 36,
    30, 37, 37, 38, 39, 40, 41, 42,
    43, 0, 44, 0, 45, 46, 47, 48,
    49, 50, 51, 45, 52, 52, 53, 48,
    45, 45, 46, 46, 54, 55, 56, 57,
    51, 58, 58, 59, 58, 60, 41, 0,
    0, 9, 0, 61, 47, 62, 63, 64,
    0, 65, 0, 22, 8, 66, 0, 9,
    22, 0, 67, 0, 0, 0, 0, 51,
    29, 9, 0, 0, 8, 9, 0, 0,
    0, 68, 23, 23, 23, 23, 23, 23,
    26, 25, 26, 26, 26, 26, 29, 29,
    29, 29, 22, 29, 22, 22, 22, 22,
    22, 0, 22, 30, 30, 30, 30, 39,
    69, 46, 45, 45, 45, 45, 45, 45,
    49, 47, 49, 49, 49, 49, 51, 51,
    70, 51, 71, 45, 46, 46, 46, 46,
    46, 9, 46, 51, 51, 51, 51, 58,
    46, 58
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 0, 0, 1, 2, 0, 3,
    4, 5, 2, 6, 7, 8, 9, 10,
    9, 10, 11, 12, 13, 14, 15, 16,
    17, 12, 18, 19, 20, 21, 21, 0,
    0, 0, 22, 23, 24, 25, 23, 25,
    25, 25, 23, 25, 25, 26, 25, 25,
    25, 25, 23, 25, 23, 25, 3, 27,
    28, 29, 29, 30, 31, 32, 33, 34,
    35, 0, 36, 0, 37, 38, 39, 39,
    39, 0, 39, 38, 40, 41, 38, 38,
    42, 42, 39, 42, 39, 42, 43, 44,
    45, 46, 46, 47, 46, 48, 0, 0,
    35, 9, 0, 49, 39, 50, 51, 52,
    0, 53, 0, 23, 8, 9, 9, 9,
    23, 0, 54, 0, 0, 0, 0, 42,
    55, 9, 0, 0, 8, 56, 0, 0,
    0, 57, 24, 24, 24, 24, 24, 24,
    24, 23, 25, 25, 25, 25, 25, 25,
    25, 25, 25, 25, 23, 23, 23, 23,
    23, 0, 23, 28, 28, 28, 28, 31,
    25, 38, 37, 37, 37, 37, 37, 37,
    37, 39, 39, 39, 39, 39, 58, 42,
    59, 60, 39, 42, 39, 39, 39, 39,
    39, 9, 39, 45, 45, 45, 45, 46,
    38, 46
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 3, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 3, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 12, 0, 7,
    -6, 0, 0, 0, 0, -14, -15, 2,
    12, 6, 4, -10, 2, 13, 1, 11,
    3, 8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 15, 2, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -5, 5,
    0, 5, 12, 0, -11, 0, 0, 0,
    0, -8, 0, 0, 0, 0, 0, -5,
    4, 5, 0, 0, -3, 0, -2, 3,
    0, -3, 0, -3, -1, -5, 0, 0,
    0, 0, -3, 0, 0, -3, -4, 0,
    0, -3, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, -3, 0,
    -1, 0, 0, 0, 0, -4, 0, 0,
    -3, 0, 0, 0, 0, -7, 0, -31,
    0, 0, -5, 0, 5, 8, 0, 0,
    -5, 3, 3, 8, 5, -4, 5, 0,
    0, -15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -9, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -7, 0, 0, 0, 0, 0, 0,
    -3, -13, 0, -10, -2, 0, 0, 0,
    0, 1, 10, 0, -8, -2, -1, 1,
    0, -4, 0, 0, -2, -19, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -20, -2, 10, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, -10, -11, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 8, 0,
    3, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 10, 2, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -9, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 5, 3, 8,
    -3, 0, 0, 5, -3, -8, -35, 2,
    7, 5, 1, -3, 0, 9, 0, 8,
    0, 8, 0, -24, 0, -3, 8, 0,
    8, -3, 5, 3, 0, 0, 1, -3,
    0, 0, -4, 20, 0, 20, 0, 8,
    0, 11, 3, 4, 0, 0, 2, 0,
    0, 8, 9, 3, -21, 20, 20, 20,
    0, 0, 0, -9, 0, 0, 0, 0,
    1, -2, 0, 2, -5, -3, -5, 2,
    0, -3, 0, 0, 0, -10, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -17, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, -6, 3, 0, -22, -3,
    0, 0, 0, 0, 1, -14, 0, -16,
    0, 0, 0, 0, -2, 0, 25, -3,
    -3, 3, 3, -2, 0, -3, 3, 0,
    0, -14, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -25, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, -3, -8,
    0, -16, -33, 1, 3, 0, 0, 0,
    0, 15, 0, 0, -9, 0, 8, 0,
    -17, -25, -17, -5, 8, 0, 0, -17,
    0, 3, -6, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 8, -31, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -17, -3, 0, 10, -5, 12, 0, -13,
    -19, 0, 0, 0, 0, 2, 0, 0,
    0, 0, 0, 2, 2, -3, -5, 0,
    -1, -1, -3, 0, 0, -2, 0, 0,
    0, -5, 0, -2, 0, -6, -5, 0,
    -6, -8, -8, -5, 0, -5, 0, -5,
    0, 0, 0, 0, -2, 0, 0, 3,
    0, 2, -3, 0, 0, 0, 0, -3,
    0, 1, 0, 0, -8, 0, 0, 0,
    0, 0, 0, 3, -2, 0, 0, 0,
    -2, 3, 3, -1, 0, 0, 0, -5,
    0, -1, 0, 0, 0, 0, 0, 1,
    0, 3, -2, 0, -3, 0, -4, 0,
    0, -2, 0, 8, 0, 0, -3, 0,
    0, 0, 0, 0, -1, 1, -2, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, -1, 0, -3, -3, 0, 0,
    0, 0, 0, 1, 0, 0, -2, 0,
    -3, -3, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, -2, -3, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, -8, -2, -8, 5, 0, 0, -5,
    3, 5, 7, 0, -6, -1, -3, 0,
    -1, -12, 3, -2, 2, -14, 3, 0,
    0, 1, -13, 0, -14, -2, -22, -2,
    0, -13, 0, 5, 7, 0, 3, 0,
    0, 0, 0, 1, 0, -5, -3, 0,
    0, 3, 3, -3, 5, -8, -5, 0,
    3, 0, 0, 0, 0, 0, 0, -3,
    0, 0, 0, -3, 0, 0, 0, 0,
    0, -1, -1, 0, -1, -3, 0, 0,
    0, 0, 0, 0, 0, -3, -3, 0,
    -2, -3, -2, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -2, 0, 0, 0, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, -2, 0, -5, 3, 0, 0, -3,
    1, 3, 3, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 2,
    0, 0, -3, 0, -3, -2, -3, 0,
    0, 0, 0, 0, 0, 0, 2, 0,
    -2, 0, 0, 0, 0, -3, -4, 0,
    0, 0, 3, 0, 5, -5, 0, 0,
    0, 0, 0, 0, 0, 8, -2, 1,
    -8, 0, 0, 7, -13, -13, -11, -5,
    3, 0, -2, -17, -5, 0, -5, 0,
    -5, 4, -5, -16, 0, -7, 0, 0,
    1, -1, 2, -2, 0, 3, 0, -8,
    -10, 0, -13, -6, -5, -6, -8, -3,
    -7, -1, -5, -7, -7, -3, -5, 3,
    -3, 2, 7, -4, -16, -6, -6, -6,
    0, 1, 0, -3, 0, 0, 0, 2,
    0, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, -1,
    0, -1, -3, 0, -4, -6, -6, -1,
    0, -8, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, 1, -2, 0,
    0, 0, 0, -3, 3, 0, 0, 0,
    -5, 0, 0, 0, 0, 3, 0, 0,
    0, 0, 0, 0, 0, 0, 12, 0,
    0, 0, 0, 0, 0, 2, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -10,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, -5, 0, 0, 0,
    0, -13, -8, 0, 0, 0, -4, -13,
    0, 0, -3, 3, 0, -7, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, -4, 0, 3, 0, 3,
    -27, 0, 0, 0, 0, -5, 0, 0,
    0, 0, 3, 0, 2, -5, -5, 0,
    -3, -3, -3, 0, 0, 0, 0, 0,
    0, -8, 0, -3, 0, -4, -3, 0,
    -6, -6, -8, -2, 0, -5, 0, -8,
    0, 0, 0, 0, 20, 0, 0, 1,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 3, 0, 0, -5, 0, 0, 0,
    0, -11, 0, 0, 0, 0, 0, -24,
    -5, 8, 8, -2, -11, 0, 3, -4,
    0, -13, -1, -3, 3, -18, -3, 3,
    0, 4, -9, -4, -9, -8, -11, 0,
    0, -15, 0, 15, 0, 0, -1, 0,
    0, 0, -1, -1, -3, -7, -8, -1,
    3, 0, 0, 0, 0, -24, -20, 3,
    5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -1, -3, -4, 0, 0, -5, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 0, -5, 0, 0, 5,
    -1, 3, 0, -6, 3, -2, -1, -7,
    -3, 0, -3, -3, -2, 0, -4, -4,
    0, 0, -2, -1, -2, -4, -3, 0,
    0, -3, 0, 3, -2, 0, -6, 0,
    0, 0, -5, 0, -4, 0, -4, -4,
    -3, 0, 0, 0, -2, 3, 2, -1,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 3, 0, -4,
    0, -2, -3, -8, -2, -2, -2, -1,
    -2, -3, -1, 0, 0, 0, 0, 0,
    -3, -2, -2, 0, 0, 0, 0, 3,
    -2, 0, -2, 0, 0, 0, -2, -3,
    -2, -2, -3, -2, -2, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    2, 10, -1, 0, -7, 0, -2, 5,
    0, -3, -11, -3, 4, 0, 0, -12,
    -4, 3, -4, 2, 0, -2, -2, -8,
    0, -4, 1, 0, 0, -4, 0, 0,
    0, 3, 3, -5, -5, 0, -4, -3,
    -4, -3, -3, 0, -4, 1, -5, -4,
    -3, 0, 0, 0, -2, 8, 3, -3,
    -12, -3, 3, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, 0, 0, -3, 0,
    0, -3, -3, 0, 0, 0, 0, -3,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, -4, 0, -5, 0, 0, 0,
    -8, 0, 2, -6, 5, 1, -2, -12,
    0, 0, -6, -3, 0, -10, -6, -7,
    0, 0, -11, -3, -10, -10, -12, 0,
    -7, 0, 2, 17, -3, 0, -6, -3,
    -1, -3, -4, -7, -5, -9, -10, -6,
    0, 0, 0, 0, 0, -3, -2, -1,
    0, -3, -3, -3, 0, 0, -2, 0,
    1, 0, 0, -18, -2, 8, 6, -6,
    -9, 0, 1, -8, 0, -13, -2, -3,
    5, -24, -3, 1, 0, 0, -17, -3,
    -13, -3, -19, 0, 0, -18, 0, 15,
    1, 0, -2, 0, 0, 0, 0, -1,
    -2, -10, -2, 0, 0, 0, 0, 0,
    0, -17, -20, 2, 0, 0, 0, 0,
    0, 0, 0, 0, -8, 0, -2, 0,
    -1, -7, -12, 0, 0, -1, -4, -8,
    -3, 0, -2, 0, 0, 0, 0, -12,
    -3, -8, -8, -2, -4, -6, -3, -4,
    0, -5, -2, -8, -4, 0, -3, -5,
    -3, -5, 0, 1, 0, -2, -8, 0,
    0, 0, 0, 0, 0, 5, 0, 0,
    -13, -5, -5, -5, 0, -5, 0, 0,
    0, 0, 3, 0, 2, -5, 10, 0,
    -3, -3, -3, 0, 0, 0, 0, 0,
    0, -8, 0, -3, 0, -4, -3, 0,
    -6, -6, -8, -2, 0, -5, 2, 10,
    0, 0, 0, 0, 20, 0, 0, 1,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 3, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, -2, -5,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, -3, -3, 0, 0, -5, -3, 0,
    0, -5, 0, 4, -1, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, 0, 0,
    -3, 0, 0, 0, 5, 2, -2, 0,
    -8, -4, 0, 8, -8, -8, -5, -5,
    10, 5, 3, -22, -2, 5, -3, 0,
    -3, 3, -3, -9, 0, -3, 3, -3,
    -2, -8, -2, 0, 0, 8, 5, 0,
    -7, 0, -14, -3, 7, -3, -10, 1,
    -3, -8, -8, -3, -5, 0, 0, 0,
    0, 10, 8, -3, -15, 3, 9, 5,
    3, 0, -4, 0, -7, 0, 2, 8,
    -6, -9, -10, -6, 8, 0, 1, -19,
    -2, 3, -4, -2, -6, 0, -6, -9,
    -4, -4, -2, 0, 0, -6, -5, -3,
    0, 8, 6, -3, -14, 0, -14, -4,
    0, -9, -15, -1, -8, -4, -8, -7,
    -8, 0, 0, 0, 0, 7, 6, -3,
    -18, 5, 9, 5, 0, 0, -3, 0,
    -5, -2, 0, -3, -5, 0, 4, -8,
    3, 0, 0, -14, 0, -3, -6, -4,
    -2, -8, -6, -8, -6, 0, -8, -3,
    -6, -5, -8, -3, 0, 0, 1, 12,
    -4, 0, -8, -3, 0, -3, -5, -6,
    -7, -7, -10, -3, -3, 0, 0, 0,
    0, -5, 0, -2, 1, -3, -3, -3,
    5, 0, -4, 0, -13, -3, 2, 5,
    -8, -9, -5, -8, 8, -3, 1, -24,
    -5, 5, -6, -4, -9, 0, -8, -11,
    -3, -3, -2, -3, -5, -8, -1, 0,
    0, 8, 7, -2, -17, 0, -15, -6,
    6, -10, -17, -5, -9, -11, -13, -8,
    -10, 0, 0, 0, -5, 5, 6, -5,
    -21, 8, 7, 6, 0, 0, 0, 0,
    -3, 0, 0, 3, -3, 5, 2, -5,
    5, 0, 0, -8, -1, 0, -1, 0,
    1, 1, -2, 0, 0, 0, 0, 0,
    0, -3, 0, 0, 0, 0, 2, 8,
    1, 0, -3, 0, 0, 0, 0, -2,
    -2, -3, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 0, 0, 0, 0, 2, 0,
    -2, 0, 10, 0, 5, 1, 1, -3,
    0, 5, 0, 0, 0, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 8, 0, 7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 4, 0, 0, 1, 0,
    0, 0, 0, 0, 0, -15, 0, -3,
    4, 0, 8, 0, 0, 25, 3, -5,
    -5, 3, 3, -2, 1, -13, 0, 0,
    12, -15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -17, 10, 36,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 10, 0, 0, 0,
    8, -15, -8, 2, 8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, 0, -5, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, -7,
    0, 0, 1, 0, 0, 3, 33, -5,
    -2, 8, 7, -7, 3, 0, 0, 3,
    3, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -33, 7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 4, -2, -3,
    7, 0, -18, 3, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, -7,
    0, 0, 0, 0, -6, -1, 0, 0,
    0, -6, 0, -3, 0, -12, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -17, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, -5, -6, 0,
    0, 0, 0, 0, 0, -4, 0, -7,
    0, 0, 0, -4, 3, -3, 0, 0,
    -7, -3, -6, 0, 0, -7, 0, -3,
    0, -12, 0, -3, 0, 0, -21, -5,
    -10, -3, -9, 0, 0, -17, 0, -7,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, -4, -5, -2, 0, 0, 0, 0,
    0, -4, -6, 0, -3, 0, 0, 0,
    0, 0, 0, 0, -6, 0, -6, 3,
    -3, 5, 0, -2, -6, -2, -4, -5,
    0, -3, -1, -2, 2, -7, -1, 0,
    0, 0, -23, -2, -4, 0, -6, 0,
    -2, -12, -2, 0, 0, -2, -2, 0,
    0, 0, 0, 2, 0, -2, -4, -2,
    0, 0, 0, 0, -2, 4, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, -2, 0, 0, 0, -5,
    3, 0, 0, 0, -7, -3, -5, 0,
    0, -7, 0, -3, 0, -12, 0, 0,
    0, 0, -25, 0, -5, -9, -13, 0,
    0, -17, 0, -2, -4, 0, 0, 0,
    0, 0, 0, 0, 0, -3, -4, -1,
    0, 0, 0, 0, 0, -4, -5, 0,
    -3, 0, 0, 0, 1, 0, 0, 4,
    -3, 0, 8, 13, -3, -3, -8, 3,
    13, 4, 6, -7, 3, 11, 3, 7,
    6, 7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 16, 12, -5,
    -3, 0, -2, 20, 11, 20, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0,
    0, 9, 11, -1, -3, 17, 12, 15,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 4, 0, 0,
    0, 0, -22, -3, -2, -10, -13, 0,
    0, -17, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 4, 0, 0, 0, 0, -22, -3,
    -2, -10, -13, 0, 0, -10, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    -6, 3, 0, -3, 2, 5, 3, -8,
    0, -1, -2, 3, 0, 2, 0, 0,
    0, 0, -6, 0, -2, -2, -5, 0,
    -2, -10, 0, 16, -3, 0, -6, -2,
    0, -2, -4, 0, -3, -7, -5, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -2, -2, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 4, 0, 0, 0, 0, -22, -3,
    -2, -10, -13, 0, 0, -17, 0, 0,
    0, 0, 0, 0, 13, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, -8, -3, -2, 8,
    -2, -3, -10, 1, -2, 1, -2, -7,
    1, 6, 1, 2, 1, 2, -6, -10,
    -3, 0, -10, -5, -7, -11, -10, 0,
    -4, -5, -3, -3, -2, -2, -3, -2,
    0, -2, -1, 4, 0, 4, -2, 0,
    0, 0, 0, 0, 3, 8, 8, 1,
    -9, -2, -2, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, -3, -3, 0, 0, -7, 0, -1,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, -3, 0, 0, 0, 0, 0,
    0, -3, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, -4,
    -3, 3, 0, -4, -5, -2, 0, -7,
    -2, -6, -2, -3, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -17, 0, 8, 0, 0, -5, 0,
    0, 0, 0, -3, 0, -3, 0, 0,
    0, 0, 0, 0, -2, -1, -5, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    -6, 0, 0, 11, -3, -8, -8, 2,
    3, 3, -1, -7, 2, 4, 2, 8,
    2, 8, -2, -7, 0, 0, -10, 0,
    0, -8, -7, 0, 0, -5, 0, -3,
    -4, 0, -4, 0, -4, 0, -2, 4,
    0, -2, -8, -3, 0, 0, 0, 0,
    0, 9, 8, 0, -8, 0, 0, 0,
    0, 0, -2, 0, -5, 0, 0, 3,
    -6, 0, 3, -3, 2, 0, 0, -8,
    0, -2, -1, 0, -3, 3, -2, 0,
    0, 0, -10, -3, -6, 0, -8, 0,
    0, -12, 0, 9, -3, 0, -5, 0,
    2, 0, -3, 0, -3, -8, 0, -3,
    0, 0, 0, 0, 0, 3, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 3, -3, 1, 0, 0,
    -3, -2, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -16, 0, 6,
    0, 0, -2, 0, 0, 0, 0, 1,
    0, -3, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 10, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -14, 1, 3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -9, 0, 0, 7, -8, -1, 3, -9,
    4, -3, -3, -9, -3, 2, -8, -5,
    -8, 0, -3, -12, 0, -5, 0, 0,
    0, -3, 3, 0, 0, 4, 1, 4,
    -10, 0, -8, -5, -6, -5, -10, -5,
    -7, -5, -6, -10, -6, 0, 0, 0,
    -3, 0, 3, -3, -6, -5, 7, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, -3, 0, 0, 0, -2, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 3, 5, 0, 0,
    0, 0, 4, 0, -6, -8, -8, -3,
    8, 0, 3, -3, 0, 7, -3, 0,
    -10, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 13, 4, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 0, 6, 0, 0, 0, 0, 4,
    0, 2, 3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 1, 2, 0,
    -1, 0, 0, 2, 2, 0, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 0, 0, 0,
    0, 5, 0, 6, 0, 0, 0, 0,
    0, -16, -15, 1, 11, 8, 4, -10,
    2, 11, 0, 9, 0, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 14, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -16, 0, 0, 0, -1, -9, 0, -15,
    -4, 0, 0, 0, -10, 1, 10, -12,
    -11, 0, 0, -11, 0, -11, -11, 0,
    -1, -34, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -24, 2, 13,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, -5, -13,
    1, -15, -23, -3, -7, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, -8, 0, -5, -6, -5, 0,
    -3, 0, 0, 0, 0, -8, 0, -8,
    0, -10, -6, 0, -3, -8, -8, -5,
    0, -10, 0, -8, -3, 0, 0, 0,
    -3, 0, 0, 1, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, 0, -4, 0,
    0, 0, 0, 20, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 0, 0, 0, 0, -22, -3,
    -2, -10, -13, 0, 0, -17, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 3,
    0, -5, 0, 2, 0, 0, -2, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 5, 0, 0,
    -2, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 71,
    .right_class_cnt     = 60,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 3,
    .bpp = 1,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t frixos_16 = {
#else
lv_font_t frixos_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 21,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if FRIXOS_16*/

