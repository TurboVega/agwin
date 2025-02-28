#pragma once

// Message declarations for notification messages

#include "agwindefs.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

typedef enum : uint8_t {
    Aw_On_Common = 101,
    Aw_On_KeyEvent,
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
    Aw_On_WindowEnabled,
    Aw_On_WindowActivated,
    Aw_On_Terminate,
} AwOnMsgType;

typedef struct tag_AwOnMsgCommon {
    AwWindow*       window;
    AwOnMsgType     msg_type;
} AwOnMsgCommon;

typedef struct tag_AwOnMsgKeyEvent {
    AwWindow*       window;
    AwOnMsgType     msg_type;
    AwKeyState      state;
} AwOnMsgKeyEvent;

typedef struct tag_AwOnMsgMouseEvent {
    AwWindow*       window;
    AwOnMsgType     msg_type;
    AwMouseState    state;
} AwOnMsgMouseEvent;

#ifdef __CPLUSPLUS
} // extern "C"
#endif
