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
    AwDoMsgCommon               do_destroy_window;
    AwDoMsgShowWindow           do_show_window;
    AwDoMsgEnableWindow         do_enable_window;
    AwDoMsgActivateWindow       do_activate_window;
    AwDoMsgCommon               do_invalidate_window;
    AwDoMsgInvalidateWindowRect do_invalidate_window_rect;
    AwDoMsgCommon               do_invalidate_client;
    AwDoMsgInvalidateClientRect do_invalidate_client_rect;
    AwDoMsgPaintWindow          do_paint_window;
    AwDoMsgCommon               do_terminate;
    AwDoMsgExit                 do_exit;

    AwOnMsgCommon               on_common;
    AwOnMsgKeyEvent             on_key_event;
    AwOnMsgCommon               on_window_resized;
    AwOnMsgCommon               on_window_moved;
    AwOnMsgCommon               on_window_created;
    AwOnMsgCommon               on_window_destroyed;
    AwOnMsgCommon               on_window_shown;
    AwOnMsgCommon               on_terminate;

} AwMsg;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
