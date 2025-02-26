// Default message handler for AW_CLASS_ICON windows

#include "agwinicon.h"
#include "agwincore.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t icon_win_msg_handler(AwMsg* msg) {
    return core_handle_message(msg);
}

#ifdef __CPLUSPLUS
} // extern "C"
#endif
