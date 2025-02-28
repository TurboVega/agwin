#pragma once

// Default message handler for AW_CLASS_MENU windows

#include "agwinmsg.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t menu_win_msg_handler(AwWindow* window, AwMsg* msg, bool* halt);

#ifdef __CPLUSPLUS
} // extern "C"
#endif
