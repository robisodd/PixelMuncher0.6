/*
v0.3: Ditched fluff (menus, animated backgrounds, etc.) to actually release this
      Spectres move randomly
v0.5: Added Pause
      Too many extra lives ends in +
*/

// ------------------------------------------------------------------------ //
//  Main Functions
// ------------------------------------------------------------------------ //
#include "main.h"
Window *main_window;

static void main_window_load(Window *window) {
  init_game(window);
  start_game();
}

static void main_window_unload(Window *window) {
  destroy_game();
}

static void init(void) {
  // Set up and push main window
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  //IF_SDK2(window_set_fullscreen(main_window, true));

  bool emulator = watch_info_get_model()==WATCH_INFO_MODEL_UNKNOWN;
  if(emulator)
    light_enable(true);
  
  //Set up other stuff
  srand(time(NULL));  // Seed randomizer
  
  //Begin
  window_stack_push(main_window, true); // Display window
}
  
static void deinit(void) {
  unload_graphics();
  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}