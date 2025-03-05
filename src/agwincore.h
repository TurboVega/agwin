/*
MIT License

Copyright (c) 2025 TurboVega

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "agwindefs.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

#pragma pack(push, 1)

#ifdef AGWIN_APP

void        core_activate_window(AwWindow* window, bool active);
void        core_close_window(AwWindow* window);
AwWindow*   core_create_window(AwApplication* app, AwWindow* parent,
                const AwClass* wclass, AwWindowStyle style, AwWindowState state,
                int16_t x, int16_t y, uint16_t width, uint16_t height,
                const char* text, uint32_t extra_data_size);
void        core_destroy_window(AwWindow* window);
void        core_enable_window(AwWindow* window, bool enabled);
void        core_exit_app(AwApplication* app);
void        core_expand_rect(AwRect* rect, int16_t delta);
void        core_expand_rect_height(AwRect* rect, int16_t delta);
void        core_expand_rect_width(AwRect* rect, int16_t delta);
void        core_expand_rect_unevenly(AwRect* rect, int16_t dleft, int16_t dtop, int16_t dright, int16_t dbottom);
AwWindow*   core_get_active_window();
AwSize      core_get_client_size(AwWindow* window);
AwRect      core_get_close_icon_rect(AwWindow* window);
AwRect      core_get_empty_rect();
AwRect      core_get_global_client_rect(AwWindow* window);
AwRect      core_get_global_title_rect(AwWindow* window);
AwRect      core_get_global_window_rect(AwWindow* window);
AwRect      core_get_intersect_rect(const AwRect* rect1, const AwRect* rect2);
AwRect      core_get_local_client_rect(AwWindow* window);
AwRect      core_get_local_window_rect(AwWindow* window);
AwRect      core_get_maximize_icon_rect(AwWindow* window);
AwRect      core_get_menu_icon_rect(AwWindow* window);
AwRect      core_get_minimize_icon_rect(AwWindow* window);
int16_t     core_get_rect_height(const AwRect* rect);
AwSize      core_get_rect_size(const AwRect* rect);
int16_t     core_get_rect_width(const AwRect* rect);
AwRect      core_get_restore_icon_rect(AwWindow* window);
const AwClass* core_get_root_class();
AwWindow*   core_get_root_window();
AwPoint     core_get_screen_center();
AwRect      core_get_screen_rect();
AwSize      core_get_screen_size();
AwRect      core_get_sizing_client_rect(AwWindow* window);
AwRect      core_get_sizing_window_rect(AwWindow* window);
AwWindow*   core_get_top_level_window(AwWindow* window);
AwRect      core_get_union_rect(const AwRect* rect1, const AwRect* rect2);
uint8_t     core_get_version();
AwSize      core_get_window_size(AwWindow* window);
int32_t     core_handle_message(AwWindow* window, AwMsg* msg, bool* halt);
void        core_initialize();
void        core_invalidate_client(AwWindow* window);
void        core_invalidate_client_rect(AwWindow* window, const AwRect* rect);
void        core_invalidate_title_bar(AwWindow* window);
void        core_invalidate_window(AwWindow* window);
void        core_invalidate_window_rect(AwWindow* window, const AwRect* rect);
int         core_load_app(const char* path);
void        core_message_loop();
void        core_move_window(AwWindow* window, int16_t x, int16_t y);
void        core_offset_rect(AwRect* rect, int16_t dx, int16_t dy);
void        core_post_message(AwMsg* msg);
int32_t     core_process_message(AwMsg* msg);
bool        core_rect_contains_point(const AwRect* rect, int16_t x, int16_t y);
void        core_resize_window(AwWindow* window, int16_t width, int16_t height);
void        core_set_text(AwWindow* window, const char* text);
void        core_show_window(AwWindow* window, bool visible);
void        core_terminate();

#endif // AGWIN_APP

typedef void        (*aw_core_activate_window)(AwWindow* window, bool active);
typedef void        (*aw_core_close_window)(AwWindow* window);
typedef AwWindow*   (*aw_core_create_window)(AwApplication* app, AwWindow* parent,
                const AwClass* wclass, AwWindowStyle style, AwWindowState state,
                int16_t x, int16_t y, uint16_t width, uint16_t height,
                const char* text, uint32_t extra_data_size);
typedef void        (*aw_core_destroy_window)(AwWindow* window);
typedef void        (*aw_core_enable_window)(AwWindow* window, bool enabled);
typedef void        (*aw_core_exit_app)(AwApplication* app);
typedef void        (*aw_core_expand_rect)(AwRect* rect, int16_t delta);
typedef void        (*aw_core_expand_rect_height)(AwRect* rect, int16_t delta);
typedef void        (*aw_core_expand_rect_width)(AwRect* rect, int16_t delta);
typedef void        (*aw_core_expand_rect_unevenly)(AwRect* rect, int16_t dleft, int16_t dtop, int16_t dright, int16_t dbottom);
typedef AwWindow*   (*aw_core_get_active_window)();
typedef AwSize      (*aw_core_get_client_size)(AwWindow* window);
typedef AwRect      (*aw_core_get_close_icon_rect)(AwWindow* window);
typedef AwRect      (*aw_core_get_empty_rect)();
typedef AwRect      (*aw_core_get_global_client_rect)(AwWindow* window);
typedef AwRect      (*aw_core_get_global_title_rect)(AwWindow* window);
typedef AwRect      (*aw_core_get_global_window_rect)(AwWindow* window);
typedef AwRect      (*aw_core_get_intersect_rect)(const AwRect* rect1, const AwRect* rect2);
typedef AwRect      (*aw_core_get_local_client_rect)(AwWindow* window);
typedef AwRect      (*aw_core_get_local_window_rect)(AwWindow* window);
typedef AwRect      (*aw_core_get_maximize_icon_rect)(AwWindow* window);
typedef AwRect      (*aw_core_get_menu_icon_rect)(AwWindow* window);
typedef AwRect      (*aw_core_get_minimize_icon_rect)(AwWindow* window);
typedef int16_t     (*aw_core_get_rect_height)(const AwRect* rect);
typedef AwSize      (*aw_core_get_rect_size)(const AwRect* rect);
typedef int16_t     (*aw_core_get_rect_width)(const AwRect* rect);
typedef AwRect      (*aw_core_get_restore_icon_rect)(AwWindow* window);
typedef const AwClass* (*aw_core_get_root_class)();
typedef AwWindow*   (*aw_core_get_root_window)();
typedef AwPoint     (*aw_core_get_screen_center)();
typedef AwRect      (*aw_core_get_screen_rect)();
typedef AwSize      (*aw_core_get_screen_size)();
typedef AwRect      (*aw_core_get_sizing_client_rect)(AwWindow* window);
typedef AwRect      (*aw_core_get_sizing_window_rect)(AwWindow* window);
typedef AwWindow*   (*aw_core_get_top_level_window)(AwWindow* window);
typedef AwRect      (*aw_core_get_union_rect)(const AwRect* rect1, const AwRect* rect2);
typedef uint8_t     (*aw_core_get_version)();
typedef AwSize      (*aw_core_get_window_size)(AwWindow* window);
typedef int32_t     (*aw_core_handle_message)(AwWindow* window, AwMsg* msg, bool* halt);
typedef void        (*aw_core_initialize)();
typedef void        (*aw_core_invalidate_client)(AwWindow* window);
typedef void        (*aw_core_invalidate_client_rect)(AwWindow* window, const AwRect* rect);
typedef void        (*aw_core_invalidate_title_bar)(AwWindow* window);
typedef void        (*aw_core_invalidate_window)(AwWindow* window);
typedef void        (*aw_core_invalidate_window_rect)(AwWindow* window, const AwRect* rect);
typedef int         (*aw_core_load_app)(const char* path);
typedef void        (*aw_core_message_loop)();
typedef void        (*aw_core_move_window)(AwWindow* window, int16_t x, int16_t y);
typedef void        (*aw_core_offset_rect)(AwRect* rect, int16_t dx, int16_t dy);
typedef void        (*aw_core_post_message)(AwMsg* msg);
typedef int32_t     (*aw_core_process_message)(AwMsg* msg);
typedef bool        (*aw_core_rect_contains_point)(const AwRect* rect, int16_t x, int16_t y);
typedef void        (*aw_core_resize_window)(AwWindow* window, int16_t width, int16_t height);
typedef void        (*aw_core_set_text)(AwWindow* window, const char* text);
typedef void        (*aw_core_show_window)(AwWindow* window, bool visible);
typedef void        (*aw_core_terminate)();

typedef struct tag_AwFcnTable {
    aw_core_activate_window             activate_window;
    aw_core_close_window                close_window;
    aw_core_create_window               create_window;
    aw_core_destroy_window              destroy_window;
    aw_core_enable_window               enable_window;
    aw_core_exit_app                    exit_app;
    aw_core_expand_rect                 expand_rect;
    aw_core_expand_rect_height          expand_rect_height;
    aw_core_expand_rect_width           expand_rect_width;
    aw_core_expand_rect_unevenly        expand_rect_unevenly;
    aw_core_get_active_window           get_active_window;
    aw_core_get_client_size             get_client_size;
    aw_core_get_close_icon_rect         get_close_icon_rect;
    aw_core_get_empty_rect              get_empty_rect;
    aw_core_get_global_client_rect      get_global_client_rect;
    aw_core_get_global_title_rect       get_global_title_rect;
    aw_core_get_global_window_rect      get_global_window_rect;
    aw_core_get_intersect_rect          get_intersect_rect;
    aw_core_get_local_client_rect       get_local_client_rect;
    aw_core_get_local_window_rect       get_local_window_rect;
    aw_core_get_maximize_icon_rect      get_maximize_icon_rect;
    aw_core_get_menu_icon_rect          get_menu_icon_rect;
    aw_core_get_minimize_icon_rect      get_minimize_icon_rect;
    aw_core_get_rect_height             get_rect_height;
    aw_core_get_rect_size               get_rect_size;
    aw_core_get_rect_width              get_rect_width;
    aw_core_get_restore_icon_rect       get_restore_icon_rect;
    aw_core_get_root_class              get_root_class;
    aw_core_get_root_window             get_root_window;
    aw_core_get_screen_center           get_screen_center;
    aw_core_get_screen_rect             get_screen_rect;
    aw_core_get_screen_size             get_screen_size;
    aw_core_get_sizing_client_rect      get_sizing_client_rect;
    aw_core_get_sizing_window_rect      get_sizing_window_rect;
    aw_core_get_top_level_window        get_top_level_window;
    aw_core_get_union_rect              get_union_rect;
    aw_core_get_version                 get_version;
    aw_core_get_window_size             get_window_size;
    aw_core_handle_message              handle_message;
    aw_core_initialize                  initialize;
    aw_core_invalidate_client           invalidate_client;
    aw_core_invalidate_client_rect      invalidate_client_rect;
    aw_core_invalidate_title_bar        invalidate_title_bar;
    aw_core_invalidate_window           invalidate_window;
    aw_core_invalidate_window_rect      invalidate_window_rect;
    aw_core_load_app                    load_app;
    aw_core_message_loop                message_loop;
    aw_core_move_window                 move_window;
    aw_core_offset_rect                 offset_rect;
    aw_core_post_message                post_message;
    aw_core_process_message             process_message;
    aw_core_rect_contains_point         rect_contains_point;
    aw_core_resize_window               resize_window;
    aw_core_set_text                    set_text;
    aw_core_show_window                 show_window;
    aw_core_terminate                   terminate;
} AwFcnTable;

typedef int (*AwStart)();

typedef struct tag_AwAppHeader {
    uint8_t         jump_start[4];      // points to C initialization code
    char            program_name[60];   // used by argc
    char            marker[3];          // contains 'WIN'
    uint8_t         version;            // version of header
    uint8_t         run_mode;           // 0=Z80, 1=ADL
    uint8_t*        load_address;       // where to load the application
    AwApplication*  app;                // points to the application structure
    const AwFcnTable* core_functions;   // points to the core function table
} AwAppHeader;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
