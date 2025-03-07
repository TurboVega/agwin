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

#include "agwindo.h"
#include "agwinon.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

#pragma pack(push, 1)

typedef union tag_AwMsg {
    AwDoMsgCommon               do_common;
    AwDoMsgResizeWindow         do_resize_window;
    AwDoMsgMoveWindow           do_move_window;
    AwDoMsgCommon               do_close_window;
    AwDoMsgShowWindow           do_show_window;
    AwDoMsgEnableWindow         do_enable_window;
    AwDoMsgActivateWindow       do_activate_window;
    AwDoMsgCommon               do_invalidate_window;
    AwDoMsgInvalidateWindowRect do_invalidate_window_rect;
    AwDoMsgCommon               do_invalidate_client;
    AwDoMsgInvalidateClientRect do_invalidate_client_rect;
    AwDoMsgPaintWindow          do_paint_window;
    AwDoMsgCommon               do_terminate;
    AwDoMsgUnloadApp            do_unload_app;

    AwOnMsgCommon               on_common;
    AwOnMsgKeyEvent             on_key_event;
    AwOnMsgMouseEvent           on_mouse_event;
    AwOnMsgCommon               on_window_resized;
    AwOnMsgCommon               on_window_moved;
    AwOnMsgCommon               on_window_created;
    AwOnMsgCommon               on_window_shown;
    AwOnMsgCommon               on_terminate;
    AwOnRealTimeClockEvent      on_real_time_clock_event;
} AwMsg;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
