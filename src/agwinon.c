#include "agwinmsg.h"
#include "agwincore.h"

void aw_emit_keyboard_action() {
    AwMsg msg;
    msg.on_input_action.window = core_get_active_window();
    msg.on_input_action.msg_type = Aw_On_KeyAction;
    core_post_message(&msg);
}

void aw_emit_mouse_action() {
    AwMsg msg;
    msg.on_input_action.window = core_get_active_window();
    msg.on_input_action.msg_type = Aw_On_MouseAction;
    core_post_message(&msg);
}
