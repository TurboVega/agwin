#pragma once

// Default message handler for AW_CLASS_MESSAGE_BOX windows

#include "agwinmsg.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t message_box_win_msg_handler(AwMsg* msg);

#ifdef __CPLUSPLUS
} // extern "C"
#endif
