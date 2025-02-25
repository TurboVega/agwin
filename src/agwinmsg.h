#pragma once

#include "agwindo.h"
#include "agwinon.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

#pragma pack(push, 1)

typedef union tag_AwMsg {
    AwDoMsgCommon               do_common;
    AwDoMsgResizeWindow         resize_window;
    AwDoMsgMoveWindow           move_window;
    AwDoMsgCommon               close_window;
    AwDoMsgCommon               destroy_window;
    AwDoMsgShowWindow           show_window;
    AwDoMsgEnableWindow         enable_window;
    AwDoMsgActivateWindow       activate_window;
    AwDoMsgCommon               invalidate_window;
    AwDoMsgInvalidateWindowRect invalidate_window_rect;
    AwDoMsgPaintWindow          paint_window;
    AwDoMsgCommon               terminate;
    AwDoMsgExit                 exit;

    AwOnMsgCommon               on_common;
    AwOnMsgInputAction          input_action;
} AwMsg;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
