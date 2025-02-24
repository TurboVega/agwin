#pragma once

#include "agwin.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

#define AW_MAJOR        0
#define AW_MINOR        1
#define AW_VERSION      ((AW_MAJOR << 4) | AW_MINOR)

typedef uint8_t (*AwGetVersion)();

typedef int (*AwGetRectWidth)(const AwRect* rect);

typedef int (*AwGetRectHeight)(const AwRect* rect);

typedef AwRect (*AwGetEmptyRect)();

typedef AwRect (*AwGetUnionRect)(const AwRect* rect1, const AwRect* rect2);

typedef AwRect (*AwGetIntersectRect)(const AwRect* rect1, const AwRect* rect2);

typedef AwWindow* (*AwGetDesktopWindow)();

typedef AwWindow* (*AwGetActiveWindow)();

typedef AwWindow* (*AwGetTopLevelWindow)(const AwWindow* window);

typedef AwWindow* (*AwCreateWindow)(AwWindow* parent, uint16_t class_id, AwWindowFlags flags,
                        int16_t x, int16_t y, uint16_t width, uint16_t height, const char* text);

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
typedef struct {
    AwGetVersion            get_version;
    AwGetRectWidth          get_rect_width;
    AwGetRectHeight         get_rect_height;
    AwGetEmptyRect          get_empty_rect;
    AwGetUnionRect          get_union_rect;
    AwGetIntersectRect      get_intersect_rect;
    AwGetDesktopWindow      get_desktop_window;
    AwGetActiveWindow       get_active_window;
    AwGetTopLevelWindow     get_top_level_window;
    AwCreateWindow          create_window;
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
