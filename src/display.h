#pragma once

#include <stdint.h>
#include "memory.h"

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define SPRITE_LENGTH 5

typedef struct {
	GContext* g_context;
  Layer* layer;
	bool pixels[DISPLAY_WIDTH][DISPLAY_HEIGHT];
} Display;


// Create a display
Display* display_create(GContext* cxt, Layer* layer);

// Destroy a display
void display_destroy(Display* display);

// Clear the display
void display_clear(Display* display);

// Draw a sprite (vx,vy) on the screen, which starts at i in memory
bool display_draw(Display* display, int8_t vx, int8_t vy, uint16_t i, Memory* mem, uint8_t height);

void display_repaint(Display* display);
