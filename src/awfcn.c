#include "awfcn.h"
#include <stdbool.h>
#include <stdlib.h>
#include <agon/vdp_key.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

#define AW_SCREEN_MODE          0       // 640x480x60Hz, 16 colors
#define AW_SCREEN_WIDTH         640
#define AW_SCREEN_HEIGHT        480
#define AW_MESSAGE_QUEUE_SIZE   64

AwWindow* desktop_window;
AwWindow* active_window;

AwMsg       message_queue[AW_MESSAGE_QUEUE_SIZE];
uint8_t     msg_count;
uint8_t     msg_read_index;
uint8_t     msg_write_index;
bool        running;
uint16_t    next_context_id = AW_CONTEXT_ID_LOW;

int32_t aw_process_message(AwMsg* msg);

const AwApplication agwin_app = { "agwin", aw_process_message, 0, 0 };

void key_event_handler(KEY_EVENT key_event)
{
	if (key_event.code == 0x7d) {
		vdp_cursor_enable(true);
		exit( 1 );						// Exit program if esc pressed
	}
	vdp_cursor_tab( 0, 0 );
//	printf( "Modifier %02x, key-code %02x, up/down %02x\n",
//			key_event.mods, key_event.code, key_event.down );
	for ( int i = 31; i >= 0; i-- ) printf( "%02x", vdp_key_bits[i] );
	return;
}

uint8_t aw_get_version() {
    return AW_VERSION;
}

int16_t aw_get_rect_width(const AwRect* rect) {
    return rect->right - rect->left;
}

int16_t aw_get_rect_height(const AwRect* rect) {
    return rect->bottom - rect->top;
}

AwSize aw_get_rect_size(const AwRect* rect) {
    return AwSize { rect->right - rect->left, rect->bottom - rect->top };
}

void aw_offset_rect(AwRect* rect, int16_t dx, int16_t dy) {
    rect->left += dx;
    rect->top += dy;
    rect->right += dx;
    rect->bottom += dy;
}

void aw_expand_rect_width(AwRect* rect, int16_t delta) {
    rect->left -= delta;
    rect->right += delta;
}

void aw_expand_rect_height(AwRect* rect, int16_t delta) {
    rect->top -= delta;
    rect->bottom += delta;
}

void aw_expand_rect(AwRect* rect, int16_t delta) {
    rect->left -= delta;
    rect->top -= delta;
    rect->right += delta;
    rect->bottom += delta;
}

void aw_expand_rect_width(AwRect* rect, int16_t dleft, int16_t dtop, int16_t dright, int16_t dbottom) {
    rect->left -= dleft;
    rect->top -= dtop;
    rect->right += dright;
    rect->bottom += dbottom;
}

AwRect aw_get_screen_rect() {
    return AwRect { 0, 0, AW_SCREEN_WIDTH, AW_SCREEN_HEIGHT };
}

AwRect aw_get_empty_rect() {
    return AwRect { 0, 0, 0, 0};
}

AwRect aw_get_union_rect(const AwRect* rect1, const AwRect* rect2) {
    return AwRect {
        min(rect1->left, rect2->left),
        min(rect1->top, rect2->top),
        max(rect1->right, rect2->right),
        max(rect1->bottom, rect2->bottom)
    };
}

AwRect aw_get_intersect_rect(const AwRect* rect1, const AwRect* rect2) {
    if (rect1->left >= rect2->right ||
        rect2->left >= rect1->right ||
        rect1->top >= rect2->bottom ||
        rect2->top >= rect1->bottom) {
        return aw_get_empty_rect();
    }

    return AwRect {
        (rect1->left > rect2->left) ? rect1->left : rect2->left,
        (rect1->top > rect2->top) ? rect1->top : rect2->top,
        (rect1->right < rect2->right) ? rect1->right : rect2->right,
        (rect1->bottom < rect2->bottom) ? rect1->bottom : rect2->bottom
    };
}

AwWindow* aw_get_desktop_window() {
    return desktop_window;
}

AwWindow* aw_get_active_window() {
    return active_window;
}

AwWindow* aw_get_top_level_window(const AwWindow* window) {
    while (true)
    {
        if (window->flags.top_level) {
            return window;
        }
        if (window->parent) {
            window = window->parent;
        } else {
            return nullptr;
        }
    }
}

uint16_t get_new_context_id() {
    // TBD - make this smarter!
    return next_context_id++;
}

AwWindow* aw_create_window(AwApplication* app, AwWindow* parent, uint16_t class_id, AwWindowFlags flags,
                        int16_t x, int16_t y, uint16_t width, uint16_t height, const char* text) {
    if ((app == nullptr) || (width < 0) || (height < 0) || (class_id == 0) || (text == 0)) {
        return; // bad parameter(s)
    }

    // Allocate memory for the window structure
    AwWindow* window = (AwWindow*) malloc(sizeof(AwWindow));
    if (window == nullptr) {
        return; // no memory
    }
    memset(window, 0, sizeof(window));

    // Allocate memory for the window text
    int32_t len = strlen(text);
    char* ptext = (char*) malloc(len + 1);
    if (ptext == nullptr) {
        free(window);
        return nullptr; // no memory
    }
    strcpy(ptext, text);
    window->text = text;
    window->text_size = len + 1;

    // Save the current VDP context
    vdp_context_save();

    // Create a new VDP context for this window
    uint16_t context_id = get_new_context_id();
    vdp_context_reset(0xFF); // all flags set

    window->parent = parent;
    window->flags = flags;
    window->app = app;
    window->class_id = class_id;
    window->context_id = context_id;

    AwRect parent_rect;
    if (parent) {
        parent_rect = get_global_client_rect(parent);
    } else {
        parent_rect = aw_get_screen_rect();
    }

    int16_t total_width = width;
    int16_t total_height = height;
    int16_t top_deco_height = 0;
    int16_t deco_thickness = 0;
    if (flags.border) {
        total_width += AW_BORDER_THICKNESS * 2;
        total_height += AW_BORDER_THICKNESS * 2;
        top_deco_height = AW_BORDER_THICKNESS;
        deco_thickness = AW_BORDER_THICKNESS;
    }
    if (flags.title_bar) {
        total_height += AW_TITLE_BAR_HEIGHT;
        top_deco_height += AW_TITLE_BAR_HEIGHT;
    }

    window->window_rect.left = parent_rect.left + x;
    window->window_rect.top = parent_rect.top + y;
    window->window_rect.right = window->window_rect.left + total_width;
    window->window_rect.bottom = window->window_rect.top + total_height;

    window->client_rect.left = window->window_rect.left + deco_thickness;
    window->client_rect.top = window->window_rect.top + top_deco_height;
    window->client_rect.right = window->window_rect.right - deco_thickness;
    window->client_rect.bottom = window->window_rect.bottom - deco_thickness;

    vdp_set_graphics_viewport();

    // Restore the VDP context
    vdp_context_restore();
}

void aw_invalidate_window(AwWindow* window) {
    window->paint_rect = window->window_rect;
}

void aw_invalidate_window_rect(AwWindow* window, const AwRect* rect) {
    AwRect extra_rect = aw_get_intersect_rect(&window->window_rect, rect);
    AwRect paint_rect = aw_get_intersect_rect(&window->paint_rect, extra_rect);
    window->paint_rect = paint_rect;
}

AwRect aw_get_global_window_rect(AwWindow* window) {
    return window->window_rect;
}

AwRect aw_get_global_client_rect(AwWindow* window) {
    return window->client_rect;
}

AwRect aw_get_local_window_rect(AwWindow* window) {
    if (window->parent) {
        return AwRect {
            window->window_rect.left - parent->client_rect.left,
            window->window_rect.top - parent->client_rect.top,
            window->window_rect.right - parent->client_rect.left,
            window->window_rect.bottom - parent->client_rect.top
        };
    } else {
        return window->window_rect;
    }
}

AwRect aw_get_local_client_rect(AwWindow* window) {
    if (window->parent) {
        return AwRect {
            window->client_rect.left - parent->client_rect.left,
            window->client_rect.top - parent->client_rect.top,
            window->client_rect.right - parent->client_rect.left,
            window->client_rect.bottom - parent->client_rect.top
        };
    } else {
        window->client_rect;
    }
}

AwRect aw_get_sizing_window_rect(AwWindow* window) {
    AwSize size = get_window_size(window);
    return AwRect { 0, 0, size.width, size.height };
}

AwRect aw_get_sizing_client_rect(AwWindow* window) {
    AwSize size = get_client_size(window);
    return AwRect { 0, 0, size.width, size.height };
}

AwSize get_window_size(AwWindow* window) {
    return get_rect_size(&window->window_rect);
}

AwSize get_client_size(AwWindow* window) {
    return get_rect_size(&window->client_rect);
}

void aw_set_text(AwWindow* window, const char* text) {

}

void aw_move_window(AwWindow* window, int16_t x, int16_t y) {

}

void aw_size_window(AwWindow* window, int16_t width, int16_t height) {

}

void aw_activate_window(AwWindow* window, bool active) {

}

void aw_enable_window(AwWindow* window, bool enabled) {

}

void aw_show_window(AwWindow* window, bool visible) {

}

void aw_close_window(AwWindow* window) {

}

void aw_destroy_window(AwWindow* window) {

}

void aw_post_message(AwMsg* msg) {

}

int32_t aw_process_message(AwMsg* msg) {

}

void aw_exit_app(AwApplication* app) {

}

void aw_terminate() {

}

const AwSystemFunctionTable aw_system_function_table = {
    aw_get_version,
    aw_get_rect_width,
    aw_get_rect_height,
    aw_get_rect_size,
    aw_offset_rect,
    aw_expand_rect_width,
    aw_expand_rect_height,
    aw_expand_rect,
    aw_expand_rect_unevenly,
    aw_get_screen_rect,
    aw_get_empty_rect,
    aw_get_union_rect,
    aw_get_intersect_rect,
    aw_get_desktop_window,
    aw_get_active_window,
    aw_get_top_level_window,
    aw_create_window,
    aw_get_global_window_rect,
    aw_get_global_client_rect,
    aw_get_local_window_rect,
    aw_get_local_client_rect,
    aw_get_sizing_window_rect,
    aw_get_sizing_client_rect,
    aw_get_window_size,
    aw_get_client_size,
    aw_set_text,
    aw_move_window,
    aw_size_window,
    aw_activate_window,
    aw_enable_window,
    aw_show_window,
    aw_close_window,
    aw_destroy_window,
    aw_post_message,
    aw_process_message,
    aw_exit_app,
    aw_terminate,
};

void aw_initialize() {
    AwWindowFlags flags;
    flags.border = 0;
    flags.title_bar = 0;
    flags.icons = 0;
    flags.sizeable = 0;
    flags.active = 1;
    flags.enabled = 1;
    flags.selected = 0;
    flags.visible = 1;

    desktop_window = aw_create_window(&agwin_app, nullptr, AW_CLASS_DESKTOP, flags,
                        0, 0, AW_SCREEN_WIDTH, AW_SCREEN_HEIGHT, "Agon Windows Desktop");
    running = true;
}

void aw_message_loop() {
    while (running) {
        if (msg_count) {
            AwMsg* msg = &message_queue[msg_read_index++];
            if (msg_read_index >= AW_MESSAGE_QUEUE_SIZE) {
                msg_read_index = 0;
            }
            msg_count--;
        }
    }
}

#ifdef __CPLUSPLUS
} // extern "C"
#endif
