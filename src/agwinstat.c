// Default message handler for AW_CLASS_STATIC_TEXT windows

#include "agwinstat.h"
#include "agwincore.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t static_text_win_msg_handler(AwMsg* msg) {
    return core_handle_message(msg);
}

#ifdef __CPLUSPLUS
} // extern "C"
#endif
