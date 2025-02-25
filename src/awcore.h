#pragma once

#include "agwin.h"
#include <stdbool.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

typedef uint8_t core_get_version();

typedef int core_get_rect_width(const AwRect* rect);

typedef int core_get_rect_height(const AwRect* rect);

typedef AwSize core_get_rect_size(const AwRect* rect);

typedef void core_offset_rect(AwRect* rect, int16_t dx, int16_t dy);

typedef void core_expand_rect_width(AwRect* rect, int16_t delta);

typedef void core_expand_rect_height(AwRect* rect, int16_t delta);

typedef void core_expand_rect(AwRect* rect, int16_t delta);

typedef void core_expand_rect_unevenly(AwRect* rect, int16_t dleft, int16_t dtop, int16_t dright, int16_t dbottom);

typedef AwRect core_get_screen_rect();

typedef AwRect core_get_empty_rect();

typedef AwRect core_get_union_rect(const AwRect* rect1, const AwRect* rect2);

typedef AwRect core_get_intersect_rect(const AwRect* rect1, const AwRect* rect2);

typedef AwWindow* core_get_desktop_window();

typedef AwWindow* core_get_active_window();

typedef AwWindow* core_get_top_level_window(const AwWindow* window);

typedef AwWindow* core_create_window(AwWindow* parent, uint16_t class_id, AwWindowFlags flags,
                        int16_t x, int16_t y, uint16_t width, uint16_t height, const char* text);

typedef void core_invalidate_window(AwWindow* window);

typedef void core_invalidate_window_rect(AwWindow* window, const AwRect* rect);

typedef AwRect core_get_global_window_rect(AwWindow* window);

typedef AwRect core_get_global_glient_rect(AwWindow* window);

typedef AwRect core_get_local_window_rect(AwWindow* window);

typedef AwRect core_get_local_client_rect(AwWindow* window);

typedef AwRect core_get_sizing_window_rect(AwWindow* window);

typedef AwRect core_get_sizing_client_rect(AwWindow* window);

typedef AwSize core_get_window_size(AwWindow* window);

typedef AwSize core_get_client_size(AwWindow* window);

typedef void core_set_text(AwWindow* window, const char* text);

typedef void core_move_window(AwWindow* window, int16_t x, int16_t y);

typedef void core_resize_window(AwWindow* window, int16_t width, int16_t height);

typedef void core_activate_window(AwWindow* window, bool active);

typedef void core_enable_window(AwWindow* window, bool enabled);

typedef void core_show_window(AwWindow* window, bool visible);

typedef void core_close_window(AwWindow* window);

typedef void core_destroy_window(AwWindow* window);

typedef void core_post_message(AwMsg* msg);

typedef int32_t core_process_message(AwMsg* msg);

typedef void core_exit_app(AwApplication* app);

typedef void core_terminate();

#ifdef __CPLUSPLUS
} // extern "C"
#endif
