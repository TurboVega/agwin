#pragma once

// Message declarations for operation messages

#ifdef __CPLUSPLUS
extern "C" {
#endif

#include "agwindefs.h"

typedef enum uint8_t {
    Aw_Do_Common = 1,
    Aw_Do_ResizeWindow,
    Aw_Do_MoveWindow,
    Aw_Do_CloseWindow,
    Aw_Do_DestroyWindow,
    Aw_Do_ShowWindow,
    Aw_Do_EnableWindow,
    Aw_Do_ActivateWindow,
    Aw_Do_Terminate,
    Aw_Do_PaintWindow,
} AwDoMsgType;

typedef struct tag_AwDoMsgCommon {
    AwWindow*       window;
    AwDoMsgType     msg_type;
} AwDoMsgCommon;

typedef struct tag_AwDoMsgPaint {
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

#ifdef __CPLUSPLUS
} // extern "C"
#endif
