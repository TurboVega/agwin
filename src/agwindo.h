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
    Aw_Do_InvalidateClient,
    Aw_Do_InvalidateClientRect,
    Aw_Do_PaintWindow,
    Aw_Do_Terminate,
    Aw_Do_UnloadApp,
} AwDoMsgType;

typedef struct tag_AwDoMsgCommon {
    AwWindow*       window;
    AwDoMsgType     msg_type;
} AwDoMsgCommon;

typedef struct tag_AwDoMsgPaintWindow {
    AwWindow*       window;
    AwDoMsgType     msg_type;
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

typedef struct tag_AwDoMsgInvalidateClientRect {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    AwRect          rect;
} AwDoMsgInvalidateClientRect;

typedef struct tag_AwDoMsgUnloadApp {
    AwWindow*       window;
    AwDoMsgType     msg_type;
    AwApplication*  app;
} AwDoMsgUnloadApp;

#ifdef __CPLUSPLUS
} // extern "C"
#endif
