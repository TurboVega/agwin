#pragma once

// Message declarations for operation messages

#include <stdbool.h>
#include "agwindefs.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

typedef enum : uint8_t {
    Aw_Do_Common = 1,
    Aw_Do_ResizeWindow,
    Aw_Do_MoveWindow,
    Aw_Do_CloseWindow,
    Aw_Do_DestroyWindow,
    Aw_Do_ShowWindow,
    Aw_Do_EnableWindow,
    Aw_Do_ActivateWindow,
    Aw_Do_InvalidateWindow,
    Aw_Do_InvalidateWindowRect,
    Aw_Do_PaintWindow,
    Aw_Do_Terminate,
    Aw_Do_Exit,
} AwDoMsgType;

typedef struct tag_AwDoMsgCommon {
    AwWindow*       window;
    AwDoMsgType     msg_type;
} AwDoMsgCommon;

typedef struct tag_AwDoMsgPaintWindow {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwRect          paint_rect;
    union {
        AwPaintFlags flags;
        uint16_t     all_flags;
    };
} AwDoMsgPaintWindow;

typedef struct tag_AwDoMsgResizeWindow {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    int16_t         width;
    int16_t         height;
} AwDoMsgResizeWindow;

typedef struct tag_AwDoMsgMoveWindow {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    AwPoint         pt;
} AwDoMsgMoveWindow;

typedef struct tag_AwDoMsgShowWindow {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    bool            visible;
} AwDoMsgShowWindow;

typedef struct tag_AwDoMsgEnableWindow {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    bool            enabled;
} AwDoMsgEnableWindow;

typedef struct tag_AwDoMsgActivateWindow {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    bool            active;
} AwDoMsgActivateWindow;

typedef struct tag_AwDoMsgInvalidateWindowRect {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    AwRect          rect;
} AwDoMsgInvalidateWindowRect;

typedef struct tag_AwDoMsgExit {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    AwApplication*  app;
} AwDoMsgExit;

void aw_resize_window(AwWindow* window, int16_t width, int16_t height);

void aw_move_window(AwWindow* window, int16_t x, int16_t y);

void aw_close_window(AwWindow* window);

void aw_destroy_window(AwWindow* window);

void aw_show_window(AwWindow* window, bool visible);

void aw_enable_window(AwWindow* window, bool enabled);

void aw_activate_window(AwWindow* window, bool active);

void aw_invalidate_window(AwWindow* window);

void aw_invalidate_window_rect(AwWindow* window, const AwRect* rect);

void aw_exit(AwApplication* app);

void aw_terminate();

#ifdef __CPLUSPLUS
} // extern "C"
#endif
