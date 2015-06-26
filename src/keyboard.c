#include "keyboard.h"

// Create a new Keyboard with no keys pressed
Keyboard* keyboard_create() {
	Keyboard* keyboard = (Keyboard*) malloc(sizeof(Keyboard));
	return keyboard; 
}

// Destroy a keyboard
void keyboard_destroy(Keyboard* keyboard) {
	free(keyboard);
}

// Get the first key that is pressed
key8_t keyboard_get_first_pressed(Keyboard* keyboard) {
	for (key8_t k = 0; k < NUM_KEYS; k++) {
		if (keyboard->keys[k]) {
			return k;
		}
	}
	return KEY_INVALID;
}

// Query if the given key is pressed
inline bool keyboard_is_pressed(Keyboard* keyboard, key8_t key) {
	return keyboard->keys[key];
}

// Press a button on the keyboard
void keyboard_press(Keyboard* keyboard, key8_t key) {
	keyboard->keys[key] = true;
}

// Release a button on the keyboard
void keyboard_release(Keyboard* keyboard, key8_t key) {
	keyboard->keys[key] = false;
}

