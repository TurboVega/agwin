#pragma once

#include "agwindefs.h"
#include <stdbool.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

void core_initialize();

void core_message_loop();

int32_t core_handle_message(AwMsg* msg);

uint8_t core_get_version();

int16_t core_get_rect_width(const AwRect* rect);

int16_t core_get_rect_height(const AwRect* rect);

AwSize core_get_rect_size(const AwRect* rect);

void core_offset_rect(AwRect* rect, int16_t dx, int16_t dy);

void core_expand_rect_width(AwRect* rect, int16_t delta);

void core_expand_rect_height(AwRect* rect, int16_t delta);

void core_expand_rect(AwRect* rect, int16_t delta);

void core_expand_rect_unevenly(AwRect* rect, int16_t dleft, int16_t dtop, int16_t dright, int16_t dbottom);

AwRect core_get_screen_rect();

AwRect core_get_empty_rect();

AwRect core_get_union_rect(const AwRect* rect1, const AwRect* rect2);

AwRect core_get_intersect_rect(const AwRect* rect1, const AwRect* rect2);

AwWindow* core_get_desktop_window();

AwWindow* core_get_active_window();

AwWindow* core_get_top_level_window(AwWindow* window);

AwWindow* core_create_window(AwApplication* app, AwWindow* parent,
                uint16_t class_id, AwWindowFlags flags,
                int16_t x, int16_t y, uint16_t width, uint16_t height, const char* text);

void core_invalidate_window(AwWindow* window);

void core_invalidate_window_rect(AwWindow* window, const AwRect* rect);

void core_invalidate_client(AwWindow* window);

void core_invalidate_client_rect(AwWindow* window, const AwRect* rect);

AwRect core_get_global_window_rect(AwWindow* window);

AwRect core_get_global_glient_rect(AwWindow* window);

AwRect core_get_local_window_rect(AwWindow* window);

AwRect core_get_local_client_rect(AwWindow* window);

AwRect core_get_sizing_window_rect(AwWindow* window);

AwRect core_get_sizing_client_rect(AwWindow* window);

AwSize core_get_window_size(AwWindow* window);

AwSize core_get_client_size(AwWindow* window);

void core_set_text(AwWindow* window, const char* text);

void core_move_window(AwWindow* window, int16_t x, int16_t y);

void core_resize_window(AwWindow* window, int16_t width, int16_t height);

void core_activate_window(AwWindow* window, bool active);

void core_enable_window(AwWindow* window, bool enabled);

void core_show_window(AwWindow* window, bool visible);

void core_close_window(AwWindow* window);

void core_destroy_window(AwWindow* window);

void core_post_message(AwMsg* msg);

int32_t core_process_message(AwMsg* msg);

void core_exit_app(AwApplication* app);

void core_terminate();

#ifdef __CPLUSPLUS
} // extern "C"
#endif
