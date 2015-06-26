#include <pebble.h>
#include "rom.h"

// Create a new ROM from a resource and a set of key bindings
Rom* rom_create(uint32_t resId, key8_t key0, key8_t key1, key8_t key2) {
  Rom* rom = (Rom*) malloc(sizeof(Rom));
  rom->resource = resource_get_handle(resId);
  rom->inputs[INPUT_1] = key0;
  rom->inputs[INPUT_2] = key1;
  rom->inputs[INPUT_3] = key2;
  return rom;
}

// Destroy a ROM
void rom_destroy(Rom* rom) {
  free(rom);
}

// Get the key that is mapped to the given Pebble button
key8_t rom_get_mapped_key(Rom* rom, ButtonId bId) {
  switch (bId) {
    case BUTTON_ID_UP:     return rom->inputs[INPUT_1];
    case BUTTON_ID_SELECT: return rom->inputs[INPUT_2];
    case BUTTON_ID_DOWN:   return rom->inputs[INPUT_3];
    default:               return KEY_INVALID; 
  }
}