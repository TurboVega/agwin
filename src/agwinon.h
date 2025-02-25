#pragma once

// Message declarations for notification messages

#include "agwindefs.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

typedef enum uint8_t {
    Aw_On_Common = 101,
    Aw_On_KeyAction,
    Aw_On_KeyDown,
    Aw_On_KeyRepeat,
    Aw_On_KeyChar,
    Aw_On_KeyUp,
    Aw_On_MouseAction,
    Aw_On_LeftButtonDown,
    Aw_On_LeftButtonUp,
    Aw_On_LeftButtonClick,
    Aw_On_LeftButtonDoubleClick,
    Aw_On_MiddleButtonDown,
    Aw_On_MiddleButtonUp,
    Aw_On_MiddleButtonClick,
    Aw_On_MiddleButtonDoubleClick,
    Aw_On_RightButtonDown,
    Aw_On_RightButtonUp,
    Aw_On_RightButtonClick,
    Aw_On_RightButtonDoubleClick,
    Aw_On_WindowResized,
    Aw_On_WindowMoved,
    Aw_On_WindowCreated,
    Aw_On_WindowDestroyed,
    Aw_On_WindowShown,
    Aw_On_WindowHidden,
    Aw_On_Terminate,
} AwOnMsgType;

typedef struct tag_AwOnMsgCommon {
    AwWindow*       window;
    AwOnMsgType     msg_type;
} AwOnMsgCommon;

typedef struct tag_AwOnMsgInputAction {
    AwWindow*       window;
    AwOnMsgType     msg_type;
    AwInputState    state;
    AwInputAction   action;
    uint16_t        key_code;
    int16_t         key_char;
    AwPoint         mouse_screen_pt;    // Mouse location relative to entire screen
    AwPoint         mouse_window_pt;    // Mouse location relative to window beneath it
    AwPoint         mouse_client_pt;    // Mouse location relative to client area beneath it
} AwOnMsgInputAction;

#ifdef __CPLUSPLUS
} // extern "C"
#endif
