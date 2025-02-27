#include "agwincore.h"
#include "agwindefs.h"
#include "agwindesk.h"
#include "agwinedit.h"
#include "agwinicon.h"
#include "agwinlist.h"
#include "agwinlist.h"
#include "agwinmbox.h"
#include "agwinmenu.h"
#include "agwinstat.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <agon/vdp_key.h>
#include <agon/vdp_vdu.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

#define AW_SCREEN_WIDTH         640
#define AW_SCREEN_HEIGHT        480
#define AW_MESSAGE_QUEUE_SIZE   128

#define PLOT_MODE_FILLED_RECT   0x65
#define FONT_SIZE               8

AwWindow* root_window;
AwWindow* active_window;
AwRect    dirty_area;

AwMsg       message_queue[AW_MESSAGE_QUEUE_SIZE];
uint8_t     msg_count;
uint8_t     msg_read_index;
uint8_t     msg_write_index;
bool        running;
uint16_t    next_context_id = AW_CONTEXT_ID_LOW;
AwKeyState  last_key_state;
bool        key_state_changed;

int32_t core_handle_message(AwMsg* msg);

AwApplication agwin_app = { "agwin", core_handle_message, 0, 0 };

void key_event_handler(KEY_EVENT key_event) {
    if (active_window) {
        AwMsg msg;
        msg.on_key_event.window = active_window;
        msg.on_key_event.msg_type = Aw_On_KeyEvent;
        msg.on_key_event.state.key_data = key_event.key_data;
        msg.on_key_event.state.all_mods = key_event.mods;
        msg.on_key_event.state.ascii_code = key_event.ascii;
        msg.on_key_event.state.key_code = key_event.code;
        msg.on_key_event.state.down = key_event.down;
        core_post_message(&msg);
    }
}

uint8_t core_get_version() {
    return AW_VERSION;
}

int16_t core_get_rect_width(const AwRect* rect) {
    return rect->right - rect->left;
}

int16_t core_get_rect_height(const AwRect* rect) {
    return rect->bottom - rect->top;
}

AwSize core_get_rect_size(const AwRect* rect) {
    AwSize size;
    size.width = rect->right - rect->left;
    size.height = rect->bottom - rect->top;
    return size;
}

void core_offset_rect(AwRect* rect, int16_t dx, int16_t dy) {
    rect->left += dx;
    rect->top += dy;
    rect->right += dx;
    rect->bottom += dy;
}

void core_expand_rect_width(AwRect* rect, int16_t delta) {
    rect->left -= delta;
    rect->right += delta;
}

void core_expand_rect_height(AwRect* rect, int16_t delta) {
    rect->top -= delta;
    rect->bottom += delta;
}

void core_expand_rect(AwRect* rect, int16_t delta) {
    rect->left -= delta;
    rect->top -= delta;
    rect->right += delta;
    rect->bottom += delta;
}

void core_expand_rect_unevenly(AwRect* rect, int16_t dleft, int16_t dtop, int16_t dright, int16_t dbottom) {
    rect->left -= dleft;
    rect->top -= dtop;
    rect->right += dright;
    rect->bottom += dbottom;
}

AwRect core_get_screen_rect() {
    AwRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = AW_SCREEN_WIDTH;
    rect.bottom = AW_SCREEN_HEIGHT;
    return rect;
}

AwRect core_get_empty_rect() {
    AwRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = 0;
    rect.bottom = 0;
    return rect;
}

AwRect core_get_union_rect(const AwRect* rect1, const AwRect* rect2) {
    AwRect rect;
    rect.left = min(rect1->left, rect2->left);
    rect.top = min(rect1->top, rect2->top);
    rect.right = max(rect1->right, rect2->right);
    rect.bottom = max(rect1->bottom, rect2->bottom);
    return rect;
}

AwRect core_get_intersect_rect(const AwRect* rect1, const AwRect* rect2) {
    if (rect1->left >= rect2->right ||
        rect2->left >= rect1->right ||
        rect1->top >= rect2->bottom ||
        rect2->top >= rect1->bottom) {
        return core_get_empty_rect();
    }

    AwRect rect;
    rect.left = (rect1->left > rect2->left) ? rect1->left : rect2->left;
    rect.top = (rect1->top > rect2->top) ? rect1->top : rect2->top;
    rect.right = (rect1->right < rect2->right) ? rect1->right : rect2->right;
    rect.bottom = (rect1->bottom < rect2->bottom) ? rect1->bottom : rect2->bottom;
    return rect;
}

AwWindow* core_get_root_window() {
    return root_window;
}

AwWindow* core_get_active_window() {
    return active_window;
}

AwWindow* core_get_top_level_window(AwWindow* window) {
    while (true)
    {
        if (window->flags.top_level) {
            return window;
        }
        if (window->parent) {
            window = window->parent;
        } else {
            return NULL;
        }
    }
}

void core_set_text(AwWindow* window, const char* text) {
    uint32_t size = strlen(text) + 1;
    if (size <= window->text_size) {
        // Text fits in allocated space
        strcpy(window->text, text);
    } else {
        free(window->text);

        // Allocate memory for the window text
        char* ptext = (char*) malloc(size);
        if (ptext) {
            window->text = ptext;
            window->text_size = size;
            strcpy(ptext, text);
        } else {
            window->text = "";
            window->text_size = 0;
        }
    }
}

AwRect core_get_global_window_rect(AwWindow* window) {
    return window->window_rect;
}

AwRect core_get_global_title_rect(AwWindow* window) {
    AwRect rect;
    rect.left = window->client_rect.left;
    rect.top = window->window_rect.top + (window->flags.border ? AW_BORDER_THICKNESS : 0);
    rect.right = window->client_rect.right;
    rect.bottom = rect.top + AW_TITLE_BAR_HEIGHT;
    return rect;
}

AwRect core_get_global_client_rect(AwWindow* window) {
    return window->client_rect;
}

void add_more_dirt(AwRect* rect) {
    /*AwSize size = core_get_rect_size(rect);
    printf("add dirt (%hu,%hu)-(%hu,%hu) %hux%hu\r\n",
            rect->left, rect->top,
            rect->right, rect->bottom,
            size.width, size.height);*/
    AwRect screen = core_get_screen_rect();
    AwRect filth1 = core_get_intersect_rect(rect, &screen);
    AwRect filth2 = core_get_union_rect(&dirty_area, &filth1);
    dirty_area = filth2;
    /*size = core_get_rect_size(&dirty_area);
    printf("dirty (%hu,%hu)-(%hu,%hu) %hux%hu\r\n",
            dirty_area.left, dirty_area.top,
            dirty_area.right, dirty_area.bottom,
            size.width, size.height);*/
}

void core_invalidate_window(AwWindow* window) {
    window->flags.window_dirty = 1;
    add_more_dirt(&window->window_rect);
}

void core_invalidate_client(AwWindow* window) {
    window->flags.client_dirty = 1;
    add_more_dirt(&window->client_rect);
}

void core_post_message(AwMsg* msg) {
    //printf("post %p %hu\r\n", msg->do_common.window, msg->do_common.msg_type);
    if (msg_count < AW_MESSAGE_QUEUE_SIZE) {
        AwMsg* pmsg = &message_queue[msg_write_index++];
        if (msg_write_index >= AW_MESSAGE_QUEUE_SIZE) {
            msg_write_index = 0;
        }
        *pmsg = *msg;
        msg_count++;
    }
}

void core_move_window(AwWindow* window, int16_t x, int16_t y) {
    core_invalidate_window(window); // invalidate where the window was

    AwRect parent_rect;
    if (window->parent) {
        parent_rect = core_get_global_client_rect(window->parent);
    } else {
        parent_rect = core_get_screen_rect();
    }

    AwSize window_size = core_get_rect_size(&window->window_rect);
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
    window->client_rect.right = window->client_rect.left + client_size.width;
    window->client_rect.bottom = window->client_rect.top + client_size.height;

    core_invalidate_window(window); // invalidate where the window is now

    AwMsg msg;
    msg.on_window_moved.window = window;
    msg.on_window_moved.msg_type = Aw_On_WindowMoved;
    core_post_message(&msg);
}

AwRect core_get_local_window_rect(AwWindow* window) {
    if (window->parent) {
        AwRect rect;
        rect.left = window->window_rect.left - window->parent->client_rect.left;
        rect.top = window->window_rect.top - window->parent->client_rect.top;
        rect.right = window->window_rect.right - window->parent->client_rect.left;
        rect.bottom = window->window_rect.bottom - window->parent->client_rect.top;
        return rect;
    } else {
        return window->window_rect;
    }
}

void core_resize_window(AwWindow* window, int16_t width, int16_t height) {
    AwRect rect = core_get_local_window_rect(window);
    window->window_rect.right = window->window_rect.left + width;
    window->window_rect.bottom = window->window_rect.top + height;
    core_move_window(window, rect.left, rect.top);

    AwMsg msg;
    msg.on_window_resized.window = window;
    msg.on_window_resized.msg_type = Aw_On_WindowResized;
    core_post_message(&msg);
}

void core_link_child(AwWindow* parent, AwWindow* child) {
    //printf("link parent %p, child %p ... ", parent, child);
    if (parent->first_child) {
        parent->last_child->next_sibling = child;
        child->prev_sibling = parent->last_child;
    } else {
        parent->first_child = child;
        child->prev_sibling = NULL;
    }
    parent->last_child = child;
    child->next_sibling = NULL;
    //printf("end link\r\n");
}

AwWindow* core_create_window(AwApplication* app, AwWindow* parent, uint16_t class_id, AwWindowFlags flags,
                        int16_t x, int16_t y, uint16_t width, uint16_t height, const char* text) {
    if ((app == NULL) || (width < 0) || (height < 0) || (class_id == 0) || (text == 0)) {
        return 0; // bad parameter(s)
    }

    // Allocate memory for the window structure
    AwWindow* window = (AwWindow*) malloc(sizeof(AwWindow));
    if (window == NULL) {
        return 0; // no memory
    }
    memset(window, 0, sizeof(AwWindow));

    vdp_context_select(0);
    vdp_logical_scr_dims(false);

    core_set_text(window, text);
    core_link_child(parent, window);
    window->flags = flags;
    window->app = app;
    window->class_id = class_id;
    window->window_rect.right = width;
    window->window_rect.bottom = height;
    window->bg_color = AW_DFLT_BG_COLOR;
    window->fg_color = AW_DFLT_FG_COLOR;

    AwMsg msg;
    msg.on_window_created.window = window;
    msg.on_window_created.msg_type = Aw_On_WindowCreated;
    core_post_message(&msg);

    msg.on_window_resized.msg_type = Aw_On_WindowResized;
    core_post_message(&msg);

    core_move_window(window, x, y);

    return window;
}

void core_invalidate_window_rect(AwWindow* window, const AwRect* rect) {
    window->flags.window_dirty = 1;
    AwRect new_rect = *rect;
    core_offset_rect(&new_rect, window->window_rect.left, window->window_rect.top);
    AwRect extra_rect = core_get_intersect_rect(&window->window_rect, &new_rect);
    add_more_dirt(&extra_rect);
}

void core_invalidate_client_rect(AwWindow* window, const AwRect* rect) {
    window->flags.client_dirty = 1;
    AwRect new_rect = *rect;
    core_offset_rect(&new_rect, window->client_rect.left, window->client_rect.top);
    AwRect extra_rect = core_get_intersect_rect(&window->window_rect, &new_rect);
    add_more_dirt(&extra_rect);
}

void core_invalidate_title_bar(AwWindow* window) {
    window->flags.title_dirty = 1;
    AwRect title_rect = core_get_global_title_rect(window);
    add_more_dirt(&title_rect);
}

AwRect core_get_local_client_rect(AwWindow* window) {
    if (window->parent) {
        AwRect rect;
        rect.left = window->client_rect.left - window->parent->client_rect.left;
        rect.top = window->client_rect.top - window->parent->client_rect.top;
        rect.right = window->client_rect.right - window->parent->client_rect.left;
        rect.bottom = window->client_rect.bottom - window->parent->client_rect.top;
        return rect;
    } else {
        return window->client_rect;
    }
}

AwSize core_get_window_size(AwWindow* window) {
    return core_get_rect_size(&window->window_rect);
}

AwSize core_get_client_size(AwWindow* window) {
    return core_get_rect_size(&window->client_rect);
}

AwRect core_get_sizing_window_rect(AwWindow* window) {
    AwSize size = core_get_window_size(window);
    AwRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = size.width;
    rect.bottom = size.height;
    return rect;
}

AwRect core_get_sizing_client_rect(AwWindow* window) {
    AwSize size = core_get_client_size(window);
    AwRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = size.width;
    rect.bottom = size.height;
    return rect;
}

void core_activate_window(AwWindow* window, bool active) {
    if (active) {
        if (window->flags.enabled && (window != active_window)) {
            if (active_window) {
                core_activate_window(active_window, false);
            }
            window->flags.active = 1;
            active_window = window;
            core_invalidate_window(window);
        }
    } else {
        if (window == active_window) {
            window->flags.active = 0;
            active_window = NULL;
            core_invalidate_window(window);
        }
    }
}

void core_enable_window(AwWindow* window, bool enabled) {
    if (enabled) {
        if (!window->flags.enabled) {
            window->flags.enabled = 1;
            core_invalidate_window(window);
        }
    } else {
        if (window->flags.enabled) {
            window->flags.enabled = 0;
            if (window == active_window) {
                core_activate_window(active_window, false);
            }
            core_invalidate_window(window);
        }
    }
}

void core_show_window(AwWindow* window, bool visible) {
    if (visible) {
        if (!window->flags.visible) {
            window->flags.visible = 1;
            core_invalidate_window(window);
        }
    } else {
        if (window->flags.visible) {
            window->flags.visible = 0;
            core_invalidate_window(window);
        }
    }
}

void core_close_window(AwWindow* window) {
    printf("close %p\r\n", window);
}

void core_destroy_window(AwWindow* window) {
    printf("destroy %p\r\n", window);
}

int32_t core_process_message(AwMsg* msg) {
    AwApplication* app = msg->do_common.window->app;
    int32_t result = (*app->msg_handler)(msg);
    if (result == AW_MSG_UNHANDLED) {
        switch (msg->do_common.window->class_id) {
            case AW_CLASS_ROOT:         return root_win_msg_handler(msg);
            case AW_CLASS_MENU:         return menu_win_msg_handler(msg);
            case AW_CLASS_MENU_ITEM:    return menu_win_msg_handler(msg);
            case AW_CLASS_LIST:         return list_win_msg_handler(msg);
            case AW_CLASS_LIST_ITEM:    return list_win_msg_handler(msg);
            case AW_CLASS_EDIT_TEXT :   return edit_text_win_msg_handler(msg);
            case AW_CLASS_STATIC_TEXT:  return static_text_win_msg_handler(msg);
            case AW_CLASS_MESSAGE_BOX : return message_box_win_msg_handler(msg);
            case AW_CLASS_ICON:         return icon_win_msg_handler(msg);
            default: return core_handle_message(msg);
        }
    }
    return result;
}

void core_exit_app(AwApplication* app) {
    printf("exit %p\r\n", app);
}

void core_terminate() {
    printf("terminate\r\n");
}

void core_set_paint_msg(AwMsg* msg, AwWindow* window,
                bool entire, bool title, bool client) {
    AwDoMsgPaintWindow* paint_msg = &msg->do_paint_window;
    AwPaintFlags* paint_flags = &paint_msg->flags;

    paint_msg->window = window;
    paint_msg->msg_type = Aw_Do_PaintWindow;
    paint_msg->all_flags = 0;

    if (window->flags.enabled) {
        paint_flags->enabled = 1;
    }

    if (window->flags.selected) {
        paint_flags->selected = 1;
    }

    if (entire) {
        paint_flags->window = 1;
        if (window->flags.border) {
            paint_flags->border = 1;
        }
    }

    if (entire || title) {
        if (window->flags.title_bar) {
            paint_flags->title_bar = 1;
            paint_flags->title = 1;
        }
        if (window->flags.icons) {
            paint_flags->icons = 1;
        }
    }

    if (entire || client) {
        paint_flags->client = 1;
        paint_flags->background = 1;
        paint_flags->foreground = 1;
    }
}

void draw_background(AwWindow* window) {
    //printf("draw_background %p\r\n", window);
    vdp_set_graphics_colour(0, window->bg_color | 0x80);
    vdp_clear_graphics();
}

void draw_foreground(AwWindow* window) {
    //printf("draw_foreground %p\r\n", window);
    if (window == root_window) return;
    vdp_set_graphics_colour(0, window->bg_color | 0x80);
    vdp_set_graphics_colour(0, window->fg_color);
    vdp_move_to(0, 0);
    vdp_clear_graphics();
    vdp_write_at_graphics_cursor();
    if (window != active_window) {
        AwSize size = core_get_window_size(window);
        printf("Window @ (%hu,%hu)\r\n",
                window->window_rect.left, window->window_rect.top);
        printf("     size %hux%hu\r\n",
                size.width, size.height);
        size = core_get_client_size(window);
        printf("Client @ (%hu,%hu)\r\n",
                window->client_rect.left, window->client_rect.top);
        printf("     size %hux%hu\r\n",
                size.width, size.height);
    } else {
        printf("Last Key Event\r\n");
        printf("data:  %08lX\r\n", last_key_state.key_data);
        printf("code:  %02hX\r\n", last_key_state.key_code);
        printf("ascii: %02hX\r\n", last_key_state.ascii_code);
        printf("mods:  %04hX\r\n", last_key_state.all_mods);
        printf("state: %s\r\n", (last_key_state.down ? "DOWN" : "UP"));
    }
}

uint8_t get_border_color(AwWindow* window) {
    return (window->flags.active ?
            AW_DFLT_ACTIVE_BORDER_COLOR :
            AW_DFLT_INACTIVE_BORDER_COLOR);
}

uint8_t get_title_bar_color(AwWindow* window) {
    return (window->flags.active ?
            AW_DFLT_ACTIVE_TITLE_BAR_COLOR | 0x80 :
            AW_DFLT_INACTIVE_TITLE_BAR_COLOR | 0x80);
}

uint8_t get_title_color(AwWindow* window) {
    return (window->flags.active ?
            AW_DFLT_ACTIVE_TITLE_COLOR :
            AW_DFLT_INACTIVE_TITLE_COLOR);
}

void draw_border(AwWindow* window) {
    //printf("draw_border %p\r\n", window);
    vdp_set_graphics_colour(0, get_border_color(window));
    AwSize size = core_get_window_size(window);

    // horizontal top border
    vdp_move_to(0, 0);
    vdp_filled_rect(size.width-1, AW_BORDER_THICKNESS-1);

    // horizontal bottom border
    vdp_move_to(0, size.height-AW_BORDER_THICKNESS);
    vdp_filled_rect(size.width-1, size.height-1);

    // vertical left border
    vdp_move_to(0, AW_BORDER_THICKNESS);
    vdp_filled_rect(AW_BORDER_THICKNESS-1, size.height-AW_BORDER_THICKNESS);

    // vertical right border
    vdp_move_to(size.width-AW_BORDER_THICKNESS, AW_BORDER_THICKNESS);
    vdp_filled_rect(size.width-1, size.height-AW_BORDER_THICKNESS);
}

void draw_title_bar(AwWindow* window) {
    //printf("draw_title_bar %p\r\n", window);
    vdp_set_graphics_colour(0, get_title_bar_color(window));
    AwSize size = core_get_client_size(window);

    vdp_move_to(AW_BORDER_THICKNESS, AW_BORDER_THICKNESS);
    vdp_filled_rect(AW_BORDER_THICKNESS + size.width - 1,
            AW_BORDER_THICKNESS + size.height - 1);
}

void draw_title(AwWindow* window) {
    //printf("draw_title %p\r\n", window);
    vdp_set_graphics_colour(0, get_title_bar_color(window));
    vdp_set_graphics_colour(0, get_title_color(window));
    vdp_move_to(AW_BORDER_THICKNESS + 2, AW_BORDER_THICKNESS + 2);
    vdp_write_at_graphics_cursor();
    printf("%s", window->text);
}

void draw_icons(AwWindow* window) {
    //printf("draw_icons %p\r\n", window);
}

typedef struct { uint8_t A; uint8_t B; uint8_t CMD; } VDU_A_B_CMD;

static VDU_A_B_CMD vdu_set_text_viewport_via_plot = { 23, 0, 0x9C };
static VDU_A_B_CMD vdu_set_graphics_viewport_via_plot = { 23, 0, 0x9D };
static VDU_A_B_CMD vdu_set_graphics_origin_via_plot = { 23, 0, 0x9E };
static VDU_A_B_CMD vdu_move_graphics_origin_and_viewport = { 23, 0, 0x9F };

void local_vdp_set_text_viewport_via_plot( void ) { VDP_PUTS( vdu_set_text_viewport_via_plot ); }
void local_vdp_set_graphics_viewport_via_plot( void ) { VDP_PUTS( vdu_set_graphics_viewport_via_plot ); }
void local_vdp_set_graphics_origin_via_plot( void ) { VDP_PUTS( vdu_set_graphics_origin_via_plot ); }
void local_vdp_move_graphics_origin_and_viewport( void ) { VDP_PUTS( vdu_move_graphics_origin_and_viewport ); }

void core_paint_window(AwMsg* msg) {
    //printf("paint %p, flags %04hX\r\n",
    //        msg->do_paint_window.window, msg->do_paint_window.all_flags);

    AwDoMsgPaintWindow* paint_msg = &msg->do_paint_window;
    AwWindow* window = paint_msg->window;

    if (!window->flags.visible) {
        return;
    }

    AwPaintFlags* paint_flags = &paint_msg->flags;

    if (paint_flags->window) {
        vdp_context_select(0);
        vdp_context_reset(0xFF); // all flags set
        vdp_logical_scr_dims(false);
        vdp_move_to(window->window_rect.left, window->window_rect.top);
        vdp_move_to(window->window_rect.right-1, window->window_rect.bottom-1);
        local_vdp_set_graphics_viewport_via_plot();

        vdp_move_to(window->window_rect.left, window->window_rect.top);
        local_vdp_set_graphics_origin_via_plot();

        /*vdp_move_to(window->window_rect.left/FONT_SIZE,
                    window->window_rect.top/FONT_SIZE);
        vdp_move_to((window->window_rect.right/FONT_SIZE)-1,
                    (window->window_rect.bottom/FONT_SIZE)-1);*/
        //vdp_move_to(0, 0);
        //vdp_move_to(7, 5);
        //local_vdp_set_text_viewport_via_plot();
        //vdp_cursor_tab(0, 0);

        vdp_set_text_viewport(
                    (window->window_rect.left/FONT_SIZE)+1,
                    (window->window_rect.top/FONT_SIZE)+1,
                    (window->window_rect.right/FONT_SIZE)-1,
                    (window->window_rect.bottom/FONT_SIZE)-1);

        if (paint_flags->border) {
            draw_border(window);
        }
        if (paint_flags->title_bar) {
            draw_title_bar(window);
        }
        if (paint_flags->title) {
            draw_title(window);
        }
        if (paint_flags->icons) {
            //draw_icons(window);
        }
    }

    if (paint_flags->client) {
        vdp_context_select(0);
        vdp_context_reset(0xFF); // all flags set
        vdp_logical_scr_dims(false);
        vdp_move_to(window->client_rect.left, window->client_rect.top);
        vdp_move_to(window->client_rect.right-1, window->client_rect.bottom-1);
        local_vdp_set_graphics_viewport_via_plot();

        vdp_move_to(window->client_rect.left, window->client_rect.top);
        local_vdp_set_graphics_origin_via_plot();

        /*vdp_move_to(window->client_rect.left/FONT_SIZE,
                    window->client_rect.top/FONT_SIZE);
        vdp_move_to((window->client_rect.right/FONT_SIZE)-1,
                    (window->client_rect.bottom/FONT_SIZE)-1);*/
        //vdp_move_to(0, 0);
        //vdp_move_to(7, 5);
        //local_vdp_set_text_viewport_via_plot();
        //vdp_cursor_tab(0, 0);

        vdp_set_text_viewport(
                    (window->client_rect.left/FONT_SIZE)+1,
                    (window->client_rect.top/FONT_SIZE)+1,
                    (window->client_rect.right/FONT_SIZE)-1,
                    (window->client_rect.bottom/FONT_SIZE)-1);

        if (paint_flags->background) {
            draw_background(window);
        }
        if (paint_flags->foreground) {
            draw_foreground(window);
        }
    }
}

int32_t core_handle_message(AwMsg* msg) {
    //printf("handle %p %hu\r\n", msg->do_common.window, (uint16_t) msg->do_common.msg_type);
    switch (msg->do_common.msg_type) {
        case Aw_Do_Common: {
            break;
        }

        case Aw_Do_ResizeWindow: {
            core_resize_window(msg->do_resize_window.window,
                 msg->do_resize_window.width, msg->do_resize_window.height);
            break;
        }

        case Aw_Do_MoveWindow: {
            core_move_window(msg->do_move_window.window,
                msg->do_move_window.pt.x, msg->do_move_window.pt.y);
            break;
        }

        case Aw_Do_CloseWindow: {
            core_close_window(msg->do_close_window.window);
            break;
        }

        case Aw_Do_DestroyWindow: {
            core_destroy_window(msg->do_destroy_window.window);
            break;
        }

        case Aw_Do_ShowWindow: {
            core_show_window(msg->do_show_window.window,
                msg->do_show_window.visible);
            break;
        }

        case Aw_Do_EnableWindow: {
            core_enable_window(msg->do_enable_window.window,
                msg->do_enable_window.enabled);
            break;
        }

        case Aw_Do_ActivateWindow: {
            core_activate_window(msg->do_activate_window.window,
                msg->do_activate_window.active);
            break;
        }

        case Aw_Do_InvalidateWindow: {
            core_invalidate_window(msg->do_invalidate_window.window);
            break;
        }

        case Aw_Do_InvalidateWindowRect: {
            core_invalidate_window_rect(msg->do_invalidate_window_rect.window,
                &msg->do_invalidate_window_rect.rect);
            break;
        }

        case Aw_Do_InvalidateClient: {
            core_invalidate_client(msg->do_invalidate_client.window);
            break;
        }

        case Aw_Do_InvalidateClientRect: {
            core_invalidate_client_rect(msg->do_invalidate_client_rect.window,
                &msg->do_invalidate_client_rect.rect);
            break;
        }

        case Aw_Do_PaintWindow: {
            core_paint_window(msg);
            break;
        }

        case Aw_Do_Terminate: {
            running = false;
            break;
        }

        case Aw_Do_Exit: {
            core_exit_app(msg->do_exit.app);
            break;
        }

        default: {
            switch (msg->on_common.msg_type) {

                case Aw_On_Common: {
                    break;
                }

                case Aw_On_KeyEvent: {
                    if (active_window && active_window->flags.title_bar) {
                        last_key_state = msg->on_key_event.state;
                        key_state_changed = true;
                        core_invalidate_client(active_window);
                    }
                    break;
                }

                case Aw_On_MouseAction: {
                    break;
                }

                case Aw_On_LeftButtonDown: {
                    break;
                }

                case Aw_On_LeftButtonUp: {
                    break;
                }

                case Aw_On_LeftButtonClick: {
                    break;
                }

                case Aw_On_LeftButtonDoubleClick: {
                    break;
                }

                case Aw_On_MiddleButtonDown: {
                    break;
                }

                case Aw_On_MiddleButtonUp: {
                    break;
                }

                case Aw_On_MiddleButtonClick: {
                    break;
                }

                case Aw_On_MiddleButtonDoubleClick: {
                    break;
                }

                case Aw_On_RightButtonDown: {
                    break;
                }

                case Aw_On_RightButtonUp: {
                    break;
                }

                case Aw_On_RightButtonClick: {
                    break;
                }

                case Aw_On_RightButtonDoubleClick: {
                    break;
                }

                case Aw_On_WindowResized: {
                    break;
                }

                case Aw_On_WindowMoved: {
                    break;
                }

                case Aw_On_WindowCreated: {
                    break;
                }

                case Aw_On_WindowDestroyed: {
                    break;
                }

                case Aw_On_WindowShown: {
                    break;
                }

                case Aw_On_WindowEnabled: {
                    break;
                }

                case Aw_On_WindowActivated: {

                }
            
                case Aw_On_Terminate: {
                    break;
                }

                default: {
                    break;
                }
            }
        }
    }

    return AW_MSG_HANDLED;
}

void core_initialize() {
    AwWindowFlags flags;
    flags.border = 1;
    flags.title_bar = 0;
    flags.icons = 0;
    flags.sizeable = 0;
    flags.active = 0;
    flags.enabled = 1;
    flags.selected = 0;
    flags.visible = 1;

    root_window = core_create_window(&agwin_app, NULL, AW_CLASS_ROOT, flags,
                        0, 0, AW_SCREEN_WIDTH, AW_SCREEN_HEIGHT, "Agon Windows Root");

    flags.border = 1;
    flags.title_bar = 1;
    flags.icons = 1;

    for (uint16_t row = 0; row < 4; row++) {
        uint16_t y = row * 116 + 5; // 5, 121, 237, 348
        for (uint16_t col = 0; col < 4; col++) {
            uint16_t x = col * 155 + 5; // 5, 160, 315, 465
            uint16_t i = row * 4 + col;
            char text[10];
            sprintf(text, "Color #%02hu", i);
            printf("%s\r\n", text);
            AwWindow* win = core_create_window(&agwin_app, root_window,
                                                AW_CLASS_USER + i, flags,
                                x, y, 150, 112, text);
            win->bg_color = i;
            win->fg_color = 15 - i;
            if (i == 10) {
                core_activate_window(win, true);
            }
        }
    }

    running = true;
}

void emit_paint_messages(AwWindow* window) {
    AwMsg msg;
    if (window->flags.window_dirty) {
        // Entire window may be dirty
        msg.do_paint_window.paint_rect =
            core_get_intersect_rect(&dirty_area, &window->window_rect);
        AwSize size = core_get_rect_size(&msg.do_paint_window.paint_rect);
        if (size.width || size.height) {
            core_set_paint_msg(&msg, window, true, false, false);
            core_post_message(&msg);
        }
    } else {
        // Part of window may be dirty
        if (window->flags.title_dirty) {
            AwRect title_rect = core_get_global_title_rect(window);
            msg.do_paint_window.paint_rect =
                core_get_intersect_rect(&dirty_area, &title_rect);
            AwSize size = core_get_rect_size(&msg.do_paint_window.paint_rect);
            if (size.width || size.height) {
                core_set_paint_msg(&msg, window, false, true, false);
                core_post_message(&msg);
            }
        }

        if (window->flags.client_dirty) {
            msg.do_paint_window.paint_rect =
                core_get_intersect_rect(&dirty_area, &window->client_rect);
            AwSize size = core_get_rect_size(&msg.do_paint_window.paint_rect);
            if (size.width || size.height) {
                core_set_paint_msg(&msg, window, false, false, true);
                core_post_message(&msg);
            }
        }
    }

    window->flags.window_dirty = 0;
    window->flags.title_dirty = 0;
    window->flags.client_dirty = 0;

    window = window->first_child;
    while (window) {
        emit_paint_messages(window);
        window = window->next_sibling;
    }
}

void core_message_loop() {
	vdp_set_key_event_handler(key_event_handler);
    while (running) {
        if (msg_count) {
            AwMsg* msg = &message_queue[msg_read_index++];
            if (msg_read_index >= AW_MESSAGE_QUEUE_SIZE) {
                msg_read_index = 0;
            }
            msg_count--;
            core_process_message(msg);
        } else {
            AwSize size = core_get_rect_size(&dirty_area);
            if (size.width || size.height) {
                // Something needs to be painted
                /*printf("dirty (%hu,%hu)-(%hu,%hu) %hux%hu\r\n",
                        dirty_area.left, dirty_area.top,
                        dirty_area.right, dirty_area.bottom,
                        size.width, size.height);*/
                emit_paint_messages(root_window);
                dirty_area = core_get_empty_rect();
            } else {
                vdp_update_key_state();
            }
        }
    }
    vdp_key_reset_interrupt();
}

#ifdef __CPLUSPLUS
} // extern "C"
#endif
