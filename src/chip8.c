#include <pebble.h>
#include "registers.h"
#include "memory.h"
#include "keyboard.h"
#include "display.h"
#include "cpu.h"
#include "rom.h"
  
#define NUM_ROMS 2

static int s_selected_index = 0;
static char* s_titles[] = { "Breakout", "Invaders"};
static Rom* s_roms[NUM_ROMS]; 
static Window *s_main_window;
static Layer *s_canvas_layer;
static SimpleMenuLayer *s_menu_layer;
static Display *s_display;
static CPU *s_proc;

static SimpleMenuSection s_sections[1];
static SimpleMenuItem s_rom_items[NUM_ROMS];

static void window_load(Window *window);

// On press, map the Button to a key (based on what the Rom is)
// Then invoke the press on the keyboard
static void key_press_handler(ClickRecognizerRef recognizer, void *context) {
  ButtonId bId = click_recognizer_get_button_id(recognizer);
  key8_t key = rom_get_mapped_key(s_proc->rom, bId);
  keyboard_press(s_proc->keyboard, key);
}

// On release, map the Button to a key (based on what the Rom is)
// Then invoke the release on the keyboard
static void key_release_handler(ClickRecognizerRef recognizer, void *context) {
  ButtonId bId = click_recognizer_get_button_id(recognizer);
  key8_t key = rom_get_mapped_key(s_proc->rom, bId);
  keyboard_release(s_proc->keyboard, key);
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  cpu_toggle_pause(s_proc);
  display_clear(s_display);
  
  layer_remove_from_parent(s_canvas_layer);
  layer_destroy(s_canvas_layer);
  s_canvas_layer = NULL;
  
  cpu_destroy(s_proc);
  display_destroy(s_display);
  s_proc = NULL;
  s_display = NULL;
  
  window_set_click_config_provider(s_main_window, NULL);
  window_load(s_main_window);
}

static void click_config_provider(void *context) {
  // Back button to pause
  window_single_click_subscribe (BUTTON_ID_BACK, back_click_handler);

  // Buttons for game  
  window_raw_click_subscribe(BUTTON_ID_UP,     key_press_handler, key_release_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_SELECT, key_press_handler, key_release_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_DOWN,   key_press_handler, key_release_handler, NULL);
}

static void layer_update_proc(Layer* layer, GContext* cxt) {
  if (s_display == NULL) {
    s_display = display_create(cxt, s_canvas_layer);
    s_proc = cpu_create(s_display);
    cpu_start(s_proc, s_roms[s_selected_index]);
  } else {
    display_repaint(s_display);
  }
}

static void menu_select_callback(int index, void* context) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  layer_remove_from_parent(simple_menu_layer_get_layer(s_menu_layer));
  window_set_click_config_provider(s_main_window, click_config_provider);
  
  // Create Canvas Layer
  s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, s_canvas_layer);
  s_selected_index = index;
  layer_set_update_proc(s_canvas_layer, layer_update_proc);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  for (int i = 0; i < NUM_ROMS; i++) {
    s_rom_items[i] = (SimpleMenuItem) {
      .title = s_titles[i],
      .callback = menu_select_callback,
    };
  }
  
  s_sections[0] = (SimpleMenuSection) {
    .title = "Roms",
    .num_items = NUM_ROMS,
    .items = s_rom_items,
  };
  
  // Create Menu Layer
  s_menu_layer = simple_menu_layer_create(window_bounds, window, s_sections, 1, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  display_destroy(s_display);
}

static void init(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(s_main_window, animated); 
  
  s_roms[0] = rom_create(RESOURCE_ID_BREAKOUT, 4, 0, 6);
  s_roms[1] = rom_create(RESOURCE_ID_INVADERS, 4, 0, 6);
}

static void deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_main_window);

  app_event_loop();
  deinit();
}
