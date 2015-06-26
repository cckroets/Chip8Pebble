#include "display.h"

#define PIXEL_SIZE 2

// Create a display
Display* display_create(GContext* cxt, Layer* layer) {
	Display* display = (Display*) malloc(sizeof(Display));
	display->g_context = cxt;
  display->layer = layer;
  display_clear(display);
	return display;
}

// Destroy a display
void display_destroy(Display* display) {
	free(display);
}

// Clear the display
void display_clear(Display* display) {
	for (int i = 0; i < DISPLAY_WIDTH; i++) {
		for (int j = 0; j < DISPLAY_HEIGHT; j++) {
			display->pixels[i][j] = 0;
		}
	}
  layer_mark_dirty(display->layer);
}

// Draw a sprite (vx,vy) on the screen, which starts at i in memory
bool display_draw(Display* display, int8_t vx, int8_t vy, uint16_t i, Memory* mem, uint8_t height) {
  vx &= 0xFF;
	if (vy < 0) {
		vy += DISPLAY_HEIGHT;
	}
  
  bool collision = false;
	int y = vy % DISPLAY_HEIGHT;
  
	for (uint16_t p = 0; p < height; p++) {
		uint8_t byte = mem_at(mem, i + p);
		for (int b = 0; b < 8; b++) {
      bool on = ((byte & (0x80 >> b))) != 0;
			int x = (vx + b) % DISPLAY_WIDTH;
			if (x < 0) {
				x += DISPLAY_WIDTH;
			}
			bool erased = on && display->pixels[x][y];
			collision |= erased;
			display->pixels[x][y] ^= on;
		}
    y = (y + 1) % DISPLAY_HEIGHT;
	}
  layer_mark_dirty(display->layer);
	return collision;
}

void display_repaint(Display* display) {
	for (int x = 0; x < DISPLAY_WIDTH; x++) {
		for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      GColor color = display->pixels[x][y] ? GColorBlack : GColorWhite;
      graphics_context_set_fill_color(display->g_context, color);
			GRect pixel = GRect(PIXEL_SIZE * x, PIXEL_SIZE * y, PIXEL_SIZE, PIXEL_SIZE);
			graphics_fill_rect(display->g_context, pixel, 0, GCornersAll );
		}
	}
}