#pragma once

// Default message handler for AW_CLASS_EDIT_TEXT windows

#include "agwinmsg.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t edit_win_msg_handler(AwMsg* msg);

#ifdef __CPLUSPLUS
} // extern "C"
#endif
