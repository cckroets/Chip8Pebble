#pragma once

#include "keyboard.h"

#define INPUT_1 0
#define INPUT_2 1
#define INPUT_3 2
#define NUM_INPUTS 3
  
typedef struct {
  ResHandle resource;
  key8_t inputs[NUM_INPUTS];
} Rom;

// Create a new ROM with the given filename, and input keys
Rom* rom_create(uint32_t resId, key8_t key0, key8_t key1, key8_t key2);

// Destroy a ROM
void rom_destroy(Rom* rom);

// Get the key that is mapped to the given Pebble button
key8_t rom_get_mapped_key(Rom* rom, ButtonId bId);