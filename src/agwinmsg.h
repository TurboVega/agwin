#pragma once

#include "agwindo.h"
#include "agwinon.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

#pragma pack(push, 1)

typedef union tag_AwMsg {
    AwDoMsgCommon           do_common;
    AwDoMsgPaintWindow      paint_window;
    AwOnMsgCommon           on_common;
    AwOnMsgInputAction      input_action;
} AwMsg;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
