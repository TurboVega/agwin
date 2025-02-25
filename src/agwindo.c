#include "agwinmsg.h"

void aw_resize_window(AwWindow* window, int16_t width, int16_t height) {
    AwMsg msg;
    msg.resize_window.window = window;
    msg.resize_window.msg_type = Aw_Do_ResizeWindow;
    msg.resize_window.width = width;
    msg.resize_window.height = height;
    aw_post_message(&msg);
}

void aw_move_window(AwWindow* window, int16_t x, int16_t y) {
    AwMsg msg;
    msg.move_window.window = window;
    msg.move_window.msg_type = Aw_Do_MoveWindow;
    msg.move_window.pt.x = x;
    msg.move_window.pt.y = y;
    aw_post_message(&msg);
}

void aw_close_window(AwWindow* window) {
    AwMsg msg;
    msg.close_window.window = window;
    msg.close_window.msg_type = Aw_Do_CloseWindow;
    aw_post_message(&msg);
}

void aw_destroy_window(AwWindow* window) {
    AwMsg msg;
    msg.destroy_window.window = window;
    msg.destroy_window.msg_type = Aw_Do_DestroyWindow;
    aw_post_message(&msg);
}

void aw_show_window(AwWindow* window, bool visible) {
    AwMsg msg;
    msg.show_window.window = window;
    msg.show_window.msg_type = Aw_Do_ShowWindow;
    msg.show_window.visible = visible;
    aw_post_message(&msg);
}

void aw_enable_window(AwWindow* window, bool enabled) {
    AwMsg msg;
    msg.enable_window.window = window;
    msg.enable_window.msg_type = Aw_Do_EnableWindow;
    msg.enable_window.enabled = enabled;
    aw_post_message(&msg);
}

void aw_activate_window(AwWindow* window, bool active) {
    AwMsg msg;
    msg.activate_window.window = window;
    msg.activate_window.msg_type = Aw_Do_ActivateWindow;
    msg.activate_window.active = active;
    aw_post_message(&msg);
}

void aw_invalidate_window(AwWindow* window) {
    AwMsg msg;
    msg.invalidate_window.window = window;
    msg.invalidate_window.msg_type = Aw_Do_InvalidateWindow;
    aw_post_message(&msg);
}

void aw_invalidate_window_rect(AwWindow* window, const AwRect* rect) {
    AwMsg msg;
    msg.invalidate_window_rect.window = window;
    msg.invalidate_window_rect.msg_type = Aw_Do_InvalidateWindowRect;
    msg.invalidate_window_rect.rect = *rect;
    aw_post_message(&msg);
}

void aw_exit(AwApplication* app) {
    AwMsg msg;
    msg.exit.window = NULL;
    msg.exit.msg_type = Aw_Do_Exit;
    msg.exit.app = app;
    aw_post_message(&msg);
}

void aw_terminate() {
    AwMsg msg;
    msg.terminate.window = NULL;
    msg.terminate.msg_type = Aw_Do_Terminate;
    aw_post_message(&msg);
 }
