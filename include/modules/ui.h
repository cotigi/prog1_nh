#include "sdl/sdl.h"

#ifndef UI_H
#define UI_H
void init_ui(GUI *gui);
void ui_handle_event(GUI *gui);
void update_ui(GUI *gui);
void draw_ui(GUI *gui);
#endif // UI_H
