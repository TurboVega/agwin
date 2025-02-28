// Default message handler for AW_CLASS_ROOT windows

#include "agwindesk.h"
#include "agwincore.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t root_win_msg_handler(AwWindow* window, AwMsg* msg, bool* halt) {
    return core_handle_message(window, msg, halt);
}

#ifdef __CPLUSPLUS
} // extern "C"
#endif
