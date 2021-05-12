/**
 * A high-level library for drawing to an EPD.
 */
#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <esp_attr.h>


/// Width of the display area in pixels.
#define EPD_WIDTH 960
/// Height of the display area in pixels.
#define EPD_HEIGHT 540


/// An area on the display.
typedef struct {
    /// Horizontal position.
    int x;
    /// Vertical position.
    int y;
    /// Area / image width, must be positive.
    int width;
    /// Area / image height, must be positive.
    int height;
} Rect_t;

/// The image drawing mode.
enum DrawMode {
    /// Draw black / grayscale image on a white display.
    BLACK_ON_WHITE = 1 << 0,
    /// "Draw with white ink" on a white display.
    WHITE_ON_WHITE = 1 << 1,
    /// Draw with white ink on a black display.
    WHITE_ON_BLACK = 1 << 2,
};

/// Font drawing flags
enum DrawFlags {
    /// Draw a background.
    ///
    /// Take the background into account
    /// when calculating the size.
    DRAW_BACKGROUND = 1 << 0,
};

/// Font properties.
typedef struct {
    /// Foreground color
    uint8_t fg_color: 4;
    /// Background color
    uint8_t bg_color: 4;
    /// Use the glyph for this codepoint for missing glyphs.
    uint32_t fallback_glyph;
    /// Additional flags, reserved for future use
    uint32_t flags;
} FontProperties;

/** Initialize the ePaper display */
void epd_init();

/** Enable display power supply. */
void epd_poweron();

/** Disable display power supply. */
void epd_poweroff();

/** Clear the whole screen by flashing it. */
void epd_clear();

void epd_poweroff_all();
/**
 * Clear an area by flashing it.
 *
 * @param area: The area to clear.
 */
void epd_clear_area(Rect_t area);

/**
 * Clear an area by flashing it.
 *
 * @param area: The area to clear.
 * @param cycles: The number of black-to-white clear cycles.
 * @param cycle_time: Length of a cycle. Default: 50 (us).
 */
void epd_clear_area_cycles(Rect_t area, int cycles, int cycle_time);

/**
 * Darken / lighten an area for a given time.
 *
 * @param area: The area to darken / lighten.
 * @param time: The time in us to apply voltage to each pixel.
 * @param color: 1: lighten, 0: darken.
 */
void epd_push_pixels(Rect_t area, short time, int color);

/**
 * Draw a picture to a given area. The image area is not cleared and assumed
 * to be white before drawing.
 *
 * @param area: The display area to draw to. `width` and `height` of the area
 *   must correspond to the image dimensions in pixels.
 * @param data: The image data, as a buffer of 4 bit wide brightness values.
 *   Pixel data is packed (two pixels per byte). A byte cannot wrap over multiple
 *   rows, images of uneven width must add a padding nibble per line.
 */
void IRAM_ATTR epd_draw_grayscale_image(Rect_t area, uint8_t *data);

/**
 * Draw a picture to a given area, with some draw mode.
 * The image area is not cleared before drawing.
 * For example, this can be used for pixel-aligned clearing.
 *
 * @param area: The display area to draw to. `width` and `height` of the area
 *   must correspond to the image dimensions in pixels.
 * @param data: The image data, as a buffer of 4 bit wide brightness values.
 *   Pixel data is packed (two pixels per byte). A byte cannot wrap over multiple
 *   rows, images of uneven width must add a padding nibble per line.
 */
void IRAM_ATTR epd_draw_image(Rect_t area, uint8_t *data, enum DrawMode mode);



void IRAM_ATTR epd_draw_frame_1bit(Rect_t area, uint8_t *ptr, enum DrawMode mode, int time);
/**
 * @returns Rectancle representing the whole screen area.
 */
Rect_t epd_full_screen();

/**
 * Draw a picture to a given framebuffer.
 *
 * @param image_area: The area to copy to. `width` and `height` of the area
 *   must correspond to the image dimensions in pixels.
 * @param image_data: The image data, as a buffer of 4 bit wide brightness values.
 *   Pixel data is packed (two pixels per byte). A byte cannot wrap over multiple
 *   rows, images of uneven width must add a padding nibble per line.
 * @param framebuffer: The framebuffer object,
 *   which must be `EPD_WIDTH / 2 * EPD_HEIGHT` large.
 */
void epd_copy_to_framebuffer(Rect_t image_area, uint8_t *image_data,
                             uint8_t *framebuffer);

/**
 * Draw a pixel a given framebuffer.
 *
 * @param x: Horizontal position in pixels.
 * @param y: Vertical position in pixels.
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 */
void epd_draw_pixel(int x, int y, uint8_t color,
                    uint8_t *framebuffer);

/**
 * Draw a horizontal line to a given framebuffer.
 *
 * @param x: Horizontal start position in pixels.
 * @param y: Vertical start position in pixels.
 * @param length: Length of the line in pixels.
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 *  which must be `EPD_WIDTH / 2 * EPD_HEIGHT` bytes large.
 */
void epd_draw_hline(int x, int y, int length, uint8_t color,
                    uint8_t *framebuffer);

/**
 * Draw a horizontal line to a given framebuffer.
 *
 * @param x: Horizontal start position in pixels.
 * @param y: Vertical start position in pixels.
 * @param length: Length of the line in pixels.
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 *  which must be `EPD_WIDTH / 2 * EPD_HEIGHT` bytes large.
 */
void epd_draw_vline(int x, int y, int length, uint8_t color,
                    uint8_t *framebuffer);

void epd_fill_circle_helper(int x0, int y0, int r,
                            int corners, int delta,
                            uint8_t color,
                            uint8_t *framebuffer);

/**
 * Draw a circle with given center and radius
 *
 * @param x0: Center-point x coordinate
 * @param y0: Center-point y coordinate
 * @param r: Radius of the circle in pixels
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 */
void epd_draw_circle(int x, int y, int r, uint8_t color,
                     uint8_t *framebuffer);

/**
 * Draw a circle with fill with given center and radius
 *
 * @param x0: Center-point x coordinate
 * @param y0: Center-point y coordinate
 * @param r: Radius of the circle in pixels
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 */
void epd_fill_circle(int x, int y, int r, uint8_t color,
                     uint8_t *framebuffer);

/**
 * Draw a rectanle with no fill color
 *
 * @param x: Top left corner x coordinate
 * @param y: Top left corner y coordinate
 * @param w: Width in pixels
 * @param h: Height in pixels
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 */
void epd_draw_rect(int x, int y, int w, int h,
                   uint8_t color, uint8_t *framebuffer);

/**
 * Draw a rectanle with fill color
 *
 * @param x: Top left corner x coordinate
 * @param y: Top left corner y coordinate
 * @param w: Width in pixels
 * @param h: Height in pixels
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 */
void epd_fill_rect(int x, int y, int w, int h,
                   uint8_t color, uint8_t *framebuffer);


/**
 * Write a line.  Bresenham's algorithm - thx wikpedia
 * @param    x0  Start point x coordinate
 * @param    y0  Start point y coordinate
 * @param    x1  End point x coordinate
 * @param    y1  End point y coordinate
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 */
/**************************************************************************/
void epd_write_line(int x0, int y0, int x1, int y1,
                    uint8_t color, uint8_t *framebuffer);

/**
 * Draw a line
 *
 * @param    x0  Start point x coordinate
 * @param    y0  Start point y coordinate
 * @param    x1  End point x coordinate
 * @param    y1  End point y coordinate
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 */
void epd_draw_line(int x0, int y0, int x1, int y1,
                   uint8_t color, uint8_t *framebuffer);

/**
 * Draw a triangle with no fill color
 *
 * @param    x0  Vertex #0 x coordinate
 * @param    y0  Vertex #0 y coordinate
 * @param    x1  Vertex #1 x coordinate
 * @param    y1  Vertex #1 y coordinate
 * @param    x2  Vertex #2 x coordinate
 * @param    y2  Vertex #2 y coordinate
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 */
void epd_draw_triangle(int x0, int y0, int x1, int y1,
                       int x2, int y2, uint8_t color,
                       uint8_t *framebuffer);

/**
 * Draw a triangle with color-fill
 *
 * @param    x0  Vertex #0 x coordinate
 * @param    y0  Vertex #0 y coordinate
 * @param    x1  Vertex #1 x coordinate
 * @param    y1  Vertex #1 y coordinate
 * @param    x2  Vertex #2 x coordinate
 * @param    y2  Vertex #2 y coordinate
 * @param color: The gray value of the line (0-255);
 * @param framebuffer: The framebuffer to draw to,
 */
void epd_fill_triangle(int x0, int y0, int x1, int y1,
                       int x2, int y2, uint8_t color,
                       uint8_t *framebuffer);


/// Font data stored PER GLYPH
typedef struct {
    uint8_t width;            ///< Bitmap dimensions in pixels
    uint8_t height;           ///< Bitmap dimensions in pixels
    uint8_t advance_x;        ///< Distance to advance cursor (x axis)
    int16_t left;             ///< X dist from cursor pos to UL corner
    int16_t top;              ///< Y dist from cursor pos to UL corner
    uint16_t compressed_size; ///< Size of the zlib-compressed font data.
    uint32_t data_offset;     ///< Pointer into GFXfont->bitmap
} GFXglyph;

/// Glyph interval structure
typedef struct {
    uint32_t first;  ///< The first unicode code point of the interval
    uint32_t last;   ///< The last unicode code point of the interval
    uint32_t offset; ///< Index of the first code point into the glyph array
} UnicodeInterval;

/// Data stored for FONT AS A WHOLE
typedef struct {
    uint8_t *bitmap;            ///< Glyph bitmaps, concatenated
    GFXglyph *glyph;            ///< Glyph array
    UnicodeInterval *intervals; ///< Valid unicode intervals for this font
    uint32_t interval_count;    ///< Number of unicode intervals.
    bool compressed;            ///< Does this font use compressed glyph bitmaps?
    uint8_t advance_y;          ///< Newline distance (y axis)
    int ascender;               ///< Maximal height of a glyph above the base line
    int descender;              ///< Maximal height of a glyph below the base line
} GFXfont;



/*!
 * Get the text bounds for string, when drawn at (x, y).
 * Set font properties to NULL to use the defaults.
 */
void get_text_bounds(const GFXfont *font, const char *string, int *x, int *y,
                     int *x1, int *y1, int *w, int *h,
                     const FontProperties *props);

/*!
 * Write text to the EPD.
 */
void writeln(const GFXfont *font, const char *string, int *cursor_x,
             int *cursor_y, uint8_t *framebuffer);

/**
 * Write text to the EPD.
 * If framebuffer is NULL, draw mode `mode` is used for direct drawing.
 */
void write_mode(const GFXfont *font, const char *string, int *cursor_x,
                int *cursor_y, uint8_t *framebuffer, enum DrawMode mode,
                const FontProperties *properties);

/**
 * Get the font glyph for a unicode code point.
 */
void get_glyph(const GFXfont *font, uint32_t code_point, GFXglyph **glyph);

/**
 * Write a (multi-line) string to the EPD.
 */
void write_string(const GFXfont *font, const char *string, int *cursor_x,
                  int *cursor_y, uint8_t *framebuffer);

#ifdef __cplusplus
}
#endif
