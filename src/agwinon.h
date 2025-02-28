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
    Aw_On_LeftButtonLongClick,
    Aw_On_LeftButtonDoubleClick,
    Aw_On_MiddleButtonDown,
    Aw_On_MiddleButtonUp,
    Aw_On_MiddleButtonClick,
    Aw_On_MiddleButtonLongClick,
    Aw_On_MiddleButtonDoubleClick,
    Aw_On_RightButtonDown,
    Aw_On_RightButtonUp,
    Aw_On_RightButtonClick,
    Aw_On_RightButtonLongClick,
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
