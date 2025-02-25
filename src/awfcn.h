#pragma once

#include "agwin.h"
#include <stdbool.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

typedef uint8_t (*AwGetVersion)();

typedef int (*AwGetRectWidth)(const AwRect* rect);

typedef int (*AwGetRectHeight)(const AwRect* rect);

typedef AwSize (*AwGetRectSize)(const AwRect* rect);

typedef void (*AwOffsetRect)(AwRect* rect, int16_t dx, int16_t dy);

typedef void (*AwExpandRectWidth)(AwRect* rect, int16_t delta);

typedef void (*AwExpandRectHeight)(AwRect* rect, int16_t delta);

typedef void (*AwExpandRect)(AwRect* rect, int16_t delta);

typedef void (*AwExpandRectUnevenly)(AwRect* rect, int16_t dleft, int16_t dtop, int16_t dright, int16_t dbottom);

typedef AwRect (*AwGetScreenRect)();

typedef AwRect (*AwGetEmptyRect)();

typedef AwRect (*AwGetUnionRect)(const AwRect* rect1, const AwRect* rect2);

typedef AwRect (*AwGetIntersectRect)(const AwRect* rect1, const AwRect* rect2);

typedef AwWindow* (*AwGetDesktopWindow)();

typedef AwWindow* (*AwGetActiveWindow)();

typedef AwWindow* (*AwGetTopLevelWindow)(const AwWindow* window);

typedef AwWindow* (*AwCreateWindow)(AwWindow* parent, uint16_t class_id, AwWindowFlags flags,
                        int16_t x, int16_t y, uint16_t width, uint16_t height, const char* text);

typedef void (*AwInvalidateWindow)(AwWindow* window);

typedef void (*AwInvalidateWindowRect)(AwWindow* window, const AwRect* rect);

typedef AwRect (*AwGetGlobalWindowRect)(AwWindow* window);

typedef AwRect (*AwGetGlobalClientRect)(AwWindow* window);

typedef AwRect (*AwGetLocalWindowRect)(AwWindow* window);

typedef AwRect (*AwGetLocalClientRect)(AwWindow* window);

typedef AwRect (*AwGetSizingWindowRect)(AwWindow* window);

typedef AwRect (*AwGetSizingClientRect)(AwWindow* window);

typedef AwSize (*AwGetWindowSize)(AwWindow* window);

typedef AwSize (*AwGetClientSize)(AwWindow* window);

typedef void (*AwSetText)(AwWindow* window, const char* text);

typedef void (*AwMoveWindow)(AwWindow* window, int16_t x, int16_t y);

typedef void (*AwSizeWindow)(AwWindow* window, int16_t width, int16_t height);

typedef void (*AwActivateWindow)(AwWindow* window, bool active);

typedef void (*AwEnableWindow)(AwWindow* window, bool enabled);

typedef void (*AwShowWindow)(AwWindow* window, bool visible);

typedef void (*AwCloseWindow)(AwWindow* window);

typedef void (*AwDestroyWindow)(AwWindow* window);

typedef void (*AwPostMessage)(AwMsg* msg);

typedef int32_t (*AwProcessMessage)(AwMsg* msg);

typedef void (*AwExitApp)(AwApplication* app);

typedef void (*AwTerminate)();

#pragma pack(push, 4)
typedef struct tag_AwSystemFunctionTable {
    AwGetVersion            get_version;
    AwGetRectWidth          get_rect_width;
    AwGetRectHeight         get_rect_height;
    AwGetRectSize           get_rect_size;
    AwOffsetRect            offset_rect;
    AwExpandRectWidth       expand_rect_width;
    AwExpandRectHeight      expand_rect_height;
    AwExpandRect            expand_rect;
    AwExpandRectUnevenly    expand_rect_unevenly;
    AwGetScreenRect         get_screen_rect;
    AwGetEmptyRect          get_empty_rect;
    AwGetUnionRect          get_union_rect;
    AwGetIntersectRect      get_intersect_rect;
    AwGetDesktopWindow      get_desktop_window;
    AwGetActiveWindow       get_active_window;
    AwGetTopLevelWindow     get_top_level_window;
    AwCreateWindow          create_window;
    AwInvalidateWindow      invalidate_window;
    AwInvalidateWindowRect  invalidate_window_rect;
    AwGetGlobalWindowRect   get_global_window_rect;
    AwGetGlobalClientRect   get_global_client_rect;
    AwGetLocalWindowRect    get_local_window_rect;
    AwGetLocalClientRect    get_local_client_rect;
    AwGetSizingWindowRect   get_sizing_window_rect;
    AwGetSizingClientRect   get_sizing_client_rect;
    AwGetWindowSize         get_window_size;
    AwGetClientSize         get_client_size;
    AwSetText               set_text;
    AwMoveWindow            move_window;
    AwSizeWindow            size_window;
    AwActivateWindow        activate_window;
    AwEnableWindow          enable_window;
    AwShowWindow            show_window;
    AwCloseWindow           close_window;
    AwDestroyWindow         destroy_window;
    AwPostMessage           post_message;
    AwProcessMessage        process_message;
    AwExitApp               exit_app;
    AwTerminate             terminate;
} AwSystemFunctionTable;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
