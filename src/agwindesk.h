#pragma once

// Default message handler for AW_CLASS_ROOT windows

#include "agwinmsg.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t root_win_msg_handler(AwMsg* msg);

#ifdef __CPLUSPLUS
} // extern "C"
#endif
