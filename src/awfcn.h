#pragma once

#include "agwin.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int get_rect_width(const AwRect* rect);

int get_rect_height(const AwRect* rect);

AwWindow* get_desktop_window();

AwWindow* get_top_level_window(const AwWindow* window);

AwWindow* create_window(AwWindow* parent, uint16_t class_id, AwWindowFlags flags,
                        uint16_t width, uint16_t height, const char* text);

void move_window(AwWindow* window, int16_t x, int16_t y);

void size_window(AwWindow* window, int16_t width, int16_t height);

void activate_window(AwWindow* window, bool active);

void enable_window(AwWindow* window, bool enabled);

void show_window(AwWindow* window, bool visible);

void destroy_window(AwWindow* window);

void post_message(AwMsg* msg);

int32_t handle_message(AwMsg* msg);

#ifdef __CPLUSPLUS
} // extern "C"
#endif
