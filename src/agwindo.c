#include "agwindo.h"

void aw_resize_window(AwWindow* window, int16_t width, int16_t height) {
    AwRect rect = aw_get_local_window_rect(window);
    window->window_rect.right = window->window_rect.left + width;
    window->window_rect.bottom = window->window_rect.top + height;
    aw_move_window(window, rect.left, rect.top);

    AwMsg msg;
    msg.window_resized.window = window;
    msg.window_resized.msg_type = AwMt_WindowResized;
    aw_post_message(&msg);
}

void aw_move_window(AwWindow* window, int16_t x, int16_t y) {
    AwRect parent_rect;
    if (window->parent) {
        parent_rect = aw_get_global_client_rect(window->parent);
    } else {
        parent_rect = aw_get_screen_rect();
    }

    AwSize window_size = aw_get_rect_size(&window->window_rect);
    AwSize client_size = window_size;
    int16_t top_deco_height = 0;
    int16_t deco_thickness = 0;
    if (window->flags.border) {
        top_deco_height = AW_BORDER_THICKNESS;
        deco_thickness = AW_BORDER_THICKNESS;
    }
    if (window->flags.title_bar) {
        top_deco_height += AW_TITLE_BAR_HEIGHT;
    }
    client_size.width -= deco_thickness * 2;
    client_size.height -= top_deco_height + deco_thickness;

    window->window_rect.left = parent_rect.left + x;
    window->window_rect.top = parent_rect.top + y;
    window->window_rect.right = window->window_rect.left + window_size.width;
    window->window_rect.bottom = window->window_rect.top + window_size.height;

    window->client_rect.left = window->window_rect.left + deco_thickness;
    window->client_rect.top = window->window_rect.top + top_deco_height;
    window->client_rect.right = window->window_rect.left + client_size.width;
    window->client_rect.bottom = window->window_rect.top + client_size.height;

    aw_invalidate_window(window);

    AwMsg msg;
    msg.window_moved.window = window;
    msg.window_moved.msg_type = AwMt_WindowMoved;
    aw_post_message(&msg);
}

void aw_close_window(AwWindow* window) {

}

void aw_destroy_window(AwWindow* window) {

}

void aw_show_window(AwWindow* window, bool visible) {
    if (visible) {
        if (!window->flags.visible) {
            window->flags.visible = 1;
            aw_invalidate_window(window);
        }
    } else {
        if (window->flags.visible) {
            window->flags.visible = 0;
            aw_invalidate_window(window);
        }
    }
}

void aw_enable_window(AwWindow* window, bool enabled) {
    if (enabled) {
        if (!window->flags.enabled) {
            window->flags.enabled = 1;
            aw_invalidate_window(window);
        }
    } else {
        if (window->flags.enabled) {
            window->flags.enabled = 0;
            if (window == active_window) {
                aw_activate_window(active_window, false);
            }
            aw_invalidate_window(window);
        }
    }
}

void aw_activate_window(AwWindow* window, bool active) {
    if (active) {
        if (window->flags.enabled && (window != active_window)) {
            if (active_window) {
                aw_activate_window(active_window, false);
            }
            window->flags.active = 1;
            active_window = window;
            aw_invalidate_window(window);
        }
    } else {
        if (window == active_window) {
            window->flags.active = 0;
            active_window = NULL;
            aw_invalidate_window(window);
        }
    }
}

void aw_invalidate_window(AwWindow* window) {
    window->paint_rect = window->window_rect;
}

void aw_invalidate_window_rect(AwWindow* window, const AwRect* rect) {
    AwRect extra_rect = aw_get_intersect_rect(&window->window_rect, rect);
    AwRect paint_rect = aw_get_intersect_rect(&window->paint_rect, &extra_rect);
    window->paint_rect = paint_rect;
}

void aw_exit(AwApplication* app) {

}

void aw_terminate();
 {

 }

