#include "agwinmsg.h"
#include "agwincore.h"

void aw_resize_window(AwWindow* window, int16_t width, int16_t height) {
    AwMsg msg;
    msg.do_resize_window.window = window;
    msg.do_resize_window.msg_type = Aw_Do_ResizeWindow;
    msg.do_resize_window.width = width;
    msg.do_resize_window.height = height;
    core_post_message(&msg);
}

void aw_move_window(AwWindow* window, int16_t x, int16_t y) {
    AwMsg msg;
    msg.do_move_window.window = window;
    msg.do_move_window.msg_type = Aw_Do_MoveWindow;
    msg.do_move_window.pt.x = x;
    msg.do_move_window.pt.y = y;
    core_post_message(&msg);
}

void aw_close_window(AwWindow* window) {
    AwMsg msg;
    msg.do_close_window.window = window;
    msg.do_close_window.msg_type = Aw_Do_CloseWindow;
    core_post_message(&msg);
}

void aw_destroy_window(AwWindow* window) {
    AwMsg msg;
    msg.do_destroy_window.window = window;
    msg.do_destroy_window.msg_type = Aw_Do_DestroyWindow;
    core_post_message(&msg);
}

void aw_show_window(AwWindow* window, bool visible) {
    AwMsg msg;
    msg.do_show_window.window = window;
    msg.do_show_window.msg_type = Aw_Do_ShowWindow;
    msg.do_show_window.visible = visible;
    core_post_message(&msg);
}

void aw_enable_window(AwWindow* window, bool enabled) {
    AwMsg msg;
    msg.do_enable_window.window = window;
    msg.do_enable_window.msg_type = Aw_Do_EnableWindow;
    msg.do_enable_window.enabled = enabled;
    core_post_message(&msg);
}

void aw_activate_window(AwWindow* window, bool active) {
    AwMsg msg;
    msg.do_activate_window.window = window;
    msg.do_activate_window.msg_type = Aw_Do_ActivateWindow;
    msg.do_activate_window.active = active;
    core_post_message(&msg);
}

void aw_invalidate_window(AwWindow* window) {
    AwMsg msg;
    msg.do_invalidate_window.window = window;
    msg.do_invalidate_window.msg_type = Aw_Do_InvalidateWindow;
    core_post_message(&msg);
}

void aw_invalidate_window_rect(AwWindow* window, const AwRect* rect) {
    AwMsg msg;
    msg.do_invalidate_window_rect.window = window;
    msg.do_invalidate_window_rect.msg_type = Aw_Do_InvalidateWindowRect;
    msg.do_invalidate_window_rect.rect = *rect;
    core_post_message(&msg);
}

void aw_exit(AwApplication* app) {
    AwMsg msg;
    msg.do_exit.window = NULL;
    msg.do_exit.msg_type = Aw_Do_Exit;
    msg.do_exit.app = app;
    core_post_message(&msg);
}

void aw_terminate() {
    AwMsg msg;
    msg.do_terminate.window = NULL;
    msg.do_terminate.msg_type = Aw_Do_Terminate;
    core_post_message(&msg);
 }
