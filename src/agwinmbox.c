#pragma once

// Default message handler for AW_CLASS_MESSAGE_BOX windows

#include "agwinmbox.h"
#include "agwincore.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t message_box_win_msg_handler(AwMsg* msg) {
    return core_handle_message(msg);
}

#ifdef __CPLUSPLUS
} // extern "C"
#endif
