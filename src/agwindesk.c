// Default message handler for AW_CLASS_ROOT windows

#include "agwindesk.h"
#include "agwincore.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t root_win_msg_handler(AwMsg* msg) {
    return core_handle_message(msg);
}

#ifdef __CPLUSPLUS
} // extern "C"
#endif
