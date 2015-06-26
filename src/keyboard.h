#pragma once

#include <pebble.h>

#define NUM_KEYS 16
#define KEY_INVALID -1

typedef int key8_t;

typedef struct {
	bool keys[NUM_KEYS];
} Keyboard;

// Create a new Keyboard with no keys pressed
Keyboard* keyboard_create();

// Destroy a keyboard
void keyboard_destroy(Keyboard* keyboard);

// Get the first key that is pressed
key8_t keyboard_get_first_pressed(Keyboard* keyboard);

// Query if the given key is pressed
bool keyboard_is_pressed(Keyboard* keyboard, key8_t key);

// Press a button on the keyboard
void keyboard_press(Keyboard* keyboard, key8_t key);

// Release a button on the keyboard
void keyboard_release(Keyboard* keyboard, key8_t key);

