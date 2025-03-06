/*
MIT License

Copyright (c) 2025 TurboVega

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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

void aw_invalidate_client(AwWindow* window) {
    AwMsg msg;
    msg.do_invalidate_client.window = window;
    msg.do_invalidate_client.msg_type = Aw_Do_InvalidateClient;
    core_post_message(&msg);
}

void aw_invalidate_client_rect(AwWindow* window, const AwRect* rect) {
    AwMsg msg;
    msg.do_invalidate_client_rect.window = window;
    msg.do_invalidate_client_rect.msg_type = Aw_Do_InvalidateClientRect;
    msg.do_invalidate_client_rect.rect = *rect;
    core_post_message(&msg);
}

void aw_unload_app(AwApplication* app) {
    AwMsg msg;
    msg.do_unload_app.window = NULL;
    msg.do_unload_app.msg_type = Aw_Do_UnloadApp;
    msg.do_unload_app.app = app;
    core_post_message(&msg);
}

void aw_terminate() {
    AwMsg msg;
    msg.do_terminate.window = NULL;
    msg.do_terminate.msg_type = Aw_Do_Terminate;
    core_post_message(&msg);
 }
