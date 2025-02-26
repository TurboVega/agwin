#pragma once

// Default message handler for AW_CLASS_LIST windows

#include "agwinmsg.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t list_win_msg_handler(AwMsg* msg);

#ifdef __CPLUSPLUS
} // extern "C"
#endif
