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
#include <time.h>
#include <agon/vdp_key.h>
#include <agon/vdp_vdu.h>
#include <mos_api.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

#define AW_SCREEN_WIDTH         640
#define AW_SCREEN_HEIGHT        480
#define AW_MESSAGE_QUEUE_SIZE   128
#define AW_APP_LIST_SIZE        16

#define PLOT_MODE_FILLED_RECT   0x65
#define FONT_SIZE               8

#define MOUSE_LONG_CLICK_TIME   30      // minimum centiseconds to be a long click
#define MOUSE_DBL_CLICK_TIME    40      // maximum centiseconds to be a double-click
#define CORNER_CLOSENESS        4       // distance from physical corner where we assume wall
#define RTC_CHECK_TICKS         90      // time between requests for RTC data

typedef struct tag_AwLoadedApp {
    AwApplication*  app;
    AwAppHeader*    hdr;
} AwLoadedApp;

AwWindow*   root_window;
AwWindow*   active_window;
AwRect      dirty_area;
AwLoadedApp app_list[AW_APP_LIST_SIZE];
uint8_t     app_count;
AwMsg       message_queue[AW_MESSAGE_QUEUE_SIZE];
uint8_t     msg_count;
uint8_t     msg_read_index;
uint8_t     msg_write_index;
bool        running;
uint16_t    next_context_id = AW_CONTEXT_ID_LOW;
AwMouseState last_mouse_state;
uint32_t    last_rtc_request;
uint32_t    last_left_btn_up;
uint32_t    last_middle_btn_up;
uint32_t    last_right_btn_up;
bool        prior_left_click;
bool        prior_middle_click;
bool        prior_right_click;
AwMouseCursor current_cursor = AwMcPointerSimple;
uint16_t    next_context_id;
uint16_t    next_buffer_id;
uint16_t    next_bitmap_id;

volatile SYSVAR * sys_var; // points to MOS system variables

int32_t core_handle_message(AwWindow* window, AwMsg* msg, bool* halt);

AwClass root_class = { "root", NULL, core_handle_message };

AwApplication agwin_app = { "agwin", 0, 0, &root_class, NULL, 1 };

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

AwWindow* core_find_window_at_point(AwWindow* window, uint16_t x, uint16_t y) {
    if (window->state.visible && window->state.enabled &&
        core_rect_contains_point(&window->window_rect, x, y)) {
        AwWindow* child = window->first_child;
        while (child) {
            AwWindow* win = core_find_window_at_point(child, x, y);
            if (win) {
                return win;
            }
            child = child->next_sibling;
        }
        return window;
    }
    return NULL;
}

int16_t get_border_thickness(AwWindow* window) {
    return (window->style.border ? AW_BORDER_THICKNESS : 0);
}

static VDU_A_B_CMD vdu_set_text_viewport_via_plot = { 23, 0, 0x9C };
static VDU_A_B_CMD vdu_set_graphics_viewport_via_plot = { 23, 0, 0x9D };
static VDU_A_B_CMD vdu_set_graphics_origin_via_plot = { 23, 0, 0x9E };
static VDU_A_B_CMD vdu_move_graphics_origin_and_viewport = { 23, 0, 0x9F };

void local_vdp_set_text_viewport_via_plot( void ) { VDP_PUTS( vdu_set_text_viewport_via_plot ); }
void local_vdp_set_graphics_viewport_via_plot( void ) { VDP_PUTS( vdu_set_graphics_viewport_via_plot ); }
void local_vdp_set_graphics_origin_via_plot( void ) { VDP_PUTS( vdu_set_graphics_origin_via_plot ); }
void local_vdp_move_graphics_origin_and_viewport( void ) { VDP_PUTS( vdu_move_graphics_origin_and_viewport ); }

void emit_rtc_messages(AwWindow* window, AwMsg* msg) {
    while (window) {
        if (window->style.need_rtc) {
            msg->on_real_time_clock_event.window = window;
            core_post_message(msg);
        }
        emit_rtc_messages(window->first_child, msg);
        window = window->next_sibling;
    }
}

void update_rtc_state() {
    if (sys_var->vdp_pflags & vdp_pflag_rtc) {
        AwMsg msg;
        msg.on_real_time_clock_event.window = NULL;
        msg.on_real_time_clock_event.msg_type = Aw_On_RealTimeClockEvent;
        msg.on_real_time_clock_event.rtc.rtc_data = ((AwRtcData*)&sys_var->rtc)->rtc_data;
        sys_var->vdp_pflags &= ~vdp_pflag_rtc;
        emit_rtc_messages(root_window, &msg);
    }
}

void update_mouse_state() {
    if (sys_var->vdp_pflags & vdp_pflag_mouse) {
        uint32_t now = (uint32_t) clock();
        time_t ticks = now - last_left_btn_up;
        AwMouseCursor possible_cursor = AwMcPointerSimple;

        AwMsg msg;
        msg.on_mouse_event.state.buttons = sys_var->mouseButtons & 0x07;
        msg.on_mouse_event.state.wheel = sys_var->mouseWheel;
        msg.on_mouse_event.state.cur_x = sys_var->mouseX;
        msg.on_mouse_event.state.cur_y = sys_var->mouseY;
        msg.on_mouse_event.state.delta_x = sys_var->mouseXDelta;
        msg.on_mouse_event.state.delta_y = sys_var->mouseYDelta;

        // Find out what window is beneath the mouse cursor
        msg.on_mouse_event.window =
            core_find_window_at_point(
                root_window,
                msg.on_mouse_event.state.cur_x,
                msg.on_mouse_event.state.cur_y);

        // Determine on what thing the mouse pointer sits
        do {
            AwRect rect = core_get_global_client_rect(msg.on_mouse_event.window);
            if (core_rect_contains_point(&rect,
                msg.on_mouse_event.state.cur_x,
                msg.on_mouse_event.state.cur_y)) {
                msg.on_mouse_event.state.target = AwMtClientArea;
                break;
            }

            rect = core_get_close_icon_rect(msg.on_mouse_event.window);
            if (core_rect_contains_point(&rect,
                msg.on_mouse_event.state.cur_x,
                msg.on_mouse_event.state.cur_y)) {
                msg.on_mouse_event.state.target = AwMtCloseIcon;
                break;
            }

            rect = core_get_minimize_icon_rect(msg.on_mouse_event.window);
            if (core_rect_contains_point(&rect,
                msg.on_mouse_event.state.cur_x,
                msg.on_mouse_event.state.cur_y)) {
                msg.on_mouse_event.state.target = AwMtMinimizeIcon;
                break;
            }

            rect = core_get_maximize_icon_rect(msg.on_mouse_event.window);
            if (core_rect_contains_point(&rect,
                msg.on_mouse_event.state.cur_x,
                msg.on_mouse_event.state.cur_y)) {
                msg.on_mouse_event.state.target = AwMtMaximizeIcon;
                break;
            }

            rect = core_get_restore_icon_rect(msg.on_mouse_event.window);
            if (core_rect_contains_point(&rect,
                msg.on_mouse_event.state.cur_x,
                msg.on_mouse_event.state.cur_y)) {
                msg.on_mouse_event.state.target = AwMtRestoreIcon;
                break;
            }

            rect = core_get_menu_icon_rect(msg.on_mouse_event.window);
            if (core_rect_contains_point(&rect,
                msg.on_mouse_event.state.cur_x,
                msg.on_mouse_event.state.cur_y)) {
                msg.on_mouse_event.state.target = AwMtMenuIcon;
                break;
            }

            rect = core_get_global_title_rect(msg.on_mouse_event.window);
            if (core_rect_contains_point(&rect,
                msg.on_mouse_event.state.cur_x,
                msg.on_mouse_event.state.cur_y)) {
                msg.on_mouse_event.state.target = AwMtTitleBar;
                break;
            }

            if (msg.on_mouse_event.window->style.sizeable) {
                rect = core_get_local_window_rect(msg.on_mouse_event.window);
                uint16_t thickness = get_border_thickness(msg.on_mouse_event.window);
                if (msg.on_mouse_event.state.cur_x >= rect.left &&
                    msg.on_mouse_event.state.cur_x < rect.left + thickness) {
                    int16_t y = msg.on_mouse_event.state.cur_y - rect.top;
                    if (y < CORNER_CLOSENESS) {
                        msg.on_mouse_event.state.target = AwMtUpperLeftCorner;
                        possible_cursor = AwMcResize2;
                        break;
                    } else if (y >= rect.bottom - CORNER_CLOSENESS) {
                        msg.on_mouse_event.state.target = AwMtLowerLeftCorner;
                        possible_cursor = AwMcResize1;
                        break;
                    } else {
                        msg.on_mouse_event.state.target = AwMtLeftBorder;
                        possible_cursor = AwMcResize4;
                        break;
                    }
                }
                if (msg.on_mouse_event.state.cur_x >= rect.right - thickness &&
                    msg.on_mouse_event.state.cur_x < rect.right) {
                    int16_t y = msg.on_mouse_event.state.cur_y - rect.top;
                    if (y < CORNER_CLOSENESS) {
                        msg.on_mouse_event.state.target = AwMtUpperRightCorner;
                        possible_cursor = AwMcResize1;
                        break;
                    } else if (y >= rect.bottom - CORNER_CLOSENESS) {
                        msg.on_mouse_event.state.target = AwMtLowerRightCorner;
                        possible_cursor = AwMcResize2;
                        break;
                    } else {
                        msg.on_mouse_event.state.target = AwMtRightBorder;
                        possible_cursor = AwMcResize4;
                        break;
                    }
                }
                if (msg.on_mouse_event.state.cur_y >= rect.top &&
                    msg.on_mouse_event.state.cur_y < rect.top + thickness) {
                    int16_t x = msg.on_mouse_event.state.cur_x - rect.left;
                    if (x < CORNER_CLOSENESS) {
                        msg.on_mouse_event.state.target = AwMtUpperLeftCorner;
                        possible_cursor = AwMcResize2;
                        break;
                    } else if (x >= rect.right - CORNER_CLOSENESS) {
                        msg.on_mouse_event.state.target = AwMtUpperRightCorner;
                        possible_cursor = AwMcResize1;
                        break;
                    } else {
                        msg.on_mouse_event.state.target = AwMtTopBorder;
                        possible_cursor = AwMcResize3;
                        break;
                    }
                }
                if (msg.on_mouse_event.state.cur_y >= rect.bottom - thickness &&
                    msg.on_mouse_event.state.cur_y < rect.bottom) {
                    int16_t x = msg.on_mouse_event.state.cur_x - rect.left;
                    if (x < CORNER_CLOSENESS) {
                        msg.on_mouse_event.state.target = AwMtLowerLeftCorner;
                        possible_cursor = AwMcResize1;
                        break;
                    } else if (x >= rect.right - CORNER_CLOSENESS) {
                        msg.on_mouse_event.state.target = AwMtLowerRightCorner;
                        possible_cursor = AwMcResize2;
                        break;
                    } else {
                        msg.on_mouse_event.state.target = AwMtBottomBorder;
                        possible_cursor = AwMcResize3;
                        break;
                    }
                }
            }
        } while (false);

        // Determine what changed (what event happened)

        // Check if/how the "start_*" members apply
        if (last_mouse_state.buttons) {
            // For continue pressed motion (drag) or end pressed motion (drop)
            msg.on_mouse_event.state.start_window = last_mouse_state.start_window;
            msg.on_mouse_event.state.start_x = last_mouse_state.start_x;
            msg.on_mouse_event.state.start_y = last_mouse_state.start_y;
            msg.on_mouse_event.state.target = last_mouse_state.target;
        } else if (msg.on_mouse_event.state.buttons) {
            // For start pressed motion (click)
            msg.on_mouse_event.state.start_window = msg.on_mouse_event.window;
            msg.on_mouse_event.state.start_x = msg.on_mouse_event.state.cur_x;
            msg.on_mouse_event.state.start_y = msg.on_mouse_event.state.cur_y;
        } else {
            // For unpressed motion (move), the "start_*" members do not apply
            msg.on_mouse_event.state.start_window = NULL;
            msg.on_mouse_event.state.start_x = 0;
            msg.on_mouse_event.state.start_y = 0;
        }

        // If the mouse position changed, and the buttons were pressed before,
        // and are still pressed now, then the mouse was "dragged". If the
        // buttons were not pressed before, and are not pressed now, then
        // the mouse was "moved". We check for "dropped" further below.
        if (msg.on_mouse_event.state.cur_x != last_mouse_state.cur_x ||
            msg.on_mouse_event.state.cur_y != last_mouse_state.cur_y) {
            if (msg.on_mouse_event.state.buttons) {
                if (last_mouse_state.buttons) {
                    msg.on_mouse_event.msg_type = Aw_On_MouseDragged;
                    core_post_message(&msg);
                }
            } else {
                if (!last_mouse_state.buttons) {
                    msg.on_mouse_event.msg_type = Aw_On_MouseMoved;
                    core_post_message(&msg);
                }
            }
        }

        // If the buttons are now all released, make sure the mouse
        // cursor is up-to-date.
        if (!msg.on_mouse_event.state.buttons) {
            if (current_cursor != possible_cursor) {
                current_cursor = possible_cursor;
                vdp_mouse_set_cursor(current_cursor);
            }
        }

        if (last_mouse_state.left != msg.on_mouse_event.state.left) {
            if (msg.on_mouse_event.state.left) {
                msg.on_mouse_event.msg_type = Aw_On_LeftButtonDown;
                core_post_message(&msg);
            } else {
                msg.on_mouse_event.msg_type = Aw_On_LeftButtonUp;
                core_post_message(&msg);
                if (prior_left_click) {
                    if (ticks <= MOUSE_DBL_CLICK_TIME) {
                        msg.on_mouse_event.msg_type = Aw_On_LeftButtonDoubleClick;
                    } else {
                        msg.on_mouse_event.msg_type = Aw_On_LeftButtonClick;
                    }
                } else {
                    if (ticks < MOUSE_LONG_CLICK_TIME) {
                        msg.on_mouse_event.msg_type = Aw_On_LeftButtonClick;
                    } else {
                        msg.on_mouse_event.msg_type = Aw_On_LeftButtonLongClick;
                    }
                    prior_left_click = true;
                }
                core_post_message(&msg);
                last_left_btn_up = now;
            }
        }

        if (last_mouse_state.middle != msg.on_mouse_event.state.middle) {
            if (msg.on_mouse_event.state.middle) {
                msg.on_mouse_event.msg_type = Aw_On_MiddleButtonDown;
                core_post_message(&msg);
            } else {
                msg.on_mouse_event.msg_type = Aw_On_MiddleButtonUp;
                core_post_message(&msg);
                if (prior_middle_click) {
                    if (ticks <= MOUSE_DBL_CLICK_TIME) {
                        msg.on_mouse_event.msg_type = Aw_On_MiddleButtonDoubleClick;
                    } else {
                        msg.on_mouse_event.msg_type = Aw_On_MiddleButtonClick;
                    }
                } else {
                    if (ticks < MOUSE_LONG_CLICK_TIME) {
                        msg.on_mouse_event.msg_type = Aw_On_MiddleButtonClick;
                    } else {
                        msg.on_mouse_event.msg_type = Aw_On_MiddleButtonLongClick;
                    }
                    prior_middle_click = true;
                }
                core_post_message(&msg);
                last_middle_btn_up = now;
            }
        }

        if (last_mouse_state.right != msg.on_mouse_event.state.right) {
            if (msg.on_mouse_event.state.right) {
                msg.on_mouse_event.msg_type = Aw_On_RightButtonDown;
                core_post_message(&msg);
            } else {
                msg.on_mouse_event.msg_type = Aw_On_RightButtonUp;
                core_post_message(&msg);
                if (prior_right_click) {
                    if (ticks <= MOUSE_DBL_CLICK_TIME) {
                        msg.on_mouse_event.msg_type = Aw_On_RightButtonDoubleClick;
                    } else {
                        msg.on_mouse_event.msg_type = Aw_On_RightButtonClick;
                    }
                } else {
                    if (ticks < MOUSE_LONG_CLICK_TIME) {
                        msg.on_mouse_event.msg_type = Aw_On_RightButtonClick;
                    } else {
                        msg.on_mouse_event.msg_type = Aw_On_RightButtonLongClick;
                    }
                    prior_right_click = true;
                }
                core_post_message(&msg);
                last_right_btn_up = now;
            }
        }

        // If the buttons were pressed before, but are not pressed now,
        // then the mouse was "dropped".
        if (last_mouse_state.buttons && !msg.on_mouse_event.state.buttons) {
            msg.on_mouse_event.msg_type = Aw_On_MouseDropped;
            core_post_message(&msg);
        }

        last_mouse_state = msg.on_mouse_event.state;
        sys_var->vdp_pflags &= ~vdp_pflag_mouse;
    }
}

const AwClass* core_get_root_class() {
    return &root_class;
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

AwSize core_get_screen_size() {
    AwSize size;
    size.width = AW_SCREEN_WIDTH;
    size.height = AW_SCREEN_HEIGHT;
    return size;
};

AwPoint core_get_screen_center() {
    AwPoint pt;
    pt.x = AW_SCREEN_WIDTH / 2;
    pt.y = AW_SCREEN_HEIGHT / 2;
    return pt;
}

AwRect core_get_empty_rect() {
    AwRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = 0;
    rect.bottom = 0;
    return rect;
}

bool core_rect_contains_point(const AwRect* rect, int16_t x, int16_t y) {
    return (x >= rect->left &&
            x < rect->right &&
            y >= rect->top &&
            y < rect->bottom);
}

AwRect core_get_union_rect(const AwRect* rect1, const AwRect* rect2) {
    if (rect1->right - rect1->left == 0) {
        return *rect2;
    }
    if (rect2->right - rect2->left == 0) {
        return *rect1;
    }
    AwRect rect;
    rect.left = min(rect1->left, rect2->left);
    rect.top = min(rect1->top, rect2->top);
    rect.right = max(rect1->right, rect2->right);
    rect.bottom = max(rect1->bottom, rect2->bottom);
    return rect;
}

AwRect core_get_intersect_rect(const AwRect* rect1, const AwRect* rect2) {
    if (rect1->right - rect1->left == 0) {
        return core_get_empty_rect();
    }
    if (rect2->right - rect2->left == 0) {
        return core_get_empty_rect();
    }
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
        if (window->style.top_level) {
            return window;
        }
        if (window->parent) {
            window = window->parent;
        } else {
            return NULL;
        }
    }
}

const char* empty_text = "";

void core_set_text(AwWindow* window, const char* text) {
    if (text == NULL) {
        if (window->text != empty_text) {
            free(window->text);
        }
        window->text = (char*) empty_text;
        window->text_size = 0;
        return;
    }

    uint32_t size = strlen(text) + 1;
    if (size <= window->text_size) {
        // Text fits in allocated space
        strcpy(window->text, text);
    } else {
        if (window->text != empty_text) {
            free(window->text);
        }

        // Allocate memory for the window text
        char* ptext = (char*) malloc(size);
        if (ptext) {
            window->text = ptext;
            window->text_size = size;
            strcpy(ptext, text);
        } else {
            window->text = (char*) empty_text;
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
    rect.top = window->window_rect.top + (window->style.border ? AW_BORDER_THICKNESS : 0);
    rect.right = window->client_rect.right;
    rect.bottom = rect.top + AW_TITLE_BAR_HEIGHT;
    return rect;
}

AwRect core_get_global_client_rect(AwWindow* window) {
    return window->client_rect;
}

void add_more_dirt(AwRect* rect) {
    AwRect screen = core_get_screen_rect();
    AwRect filth1 = core_get_intersect_rect(rect, &screen);
    AwRect filth2 = core_get_union_rect(&dirty_area, &filth1);
    dirty_area = filth2;
}

void core_invalidate_window(AwWindow* window) {
    if (window->state.minimized) {
        AwRect rect = core_get_global_title_rect(window);
        add_more_dirt(&rect);
    } else {
        add_more_dirt(&window->window_rect);
    }
}

void core_invalidate_client(AwWindow* window) {
    if (!window->state.minimized) {
        add_more_dirt(&window->client_rect);
    }
}

void core_post_message(AwMsg* msg) {
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
    if (window->style.border) {
        top_deco_height = AW_BORDER_THICKNESS;
        deco_thickness = AW_BORDER_THICKNESS;
    }
    if (window->style.title_bar) {
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
    if (parent) {
        // Non-root window
        if (parent->last_child) {
            child->prev_sibling = parent->last_child;
            parent->last_child->next_sibling = child;
        } else {
            parent->first_child = child;
            child->prev_sibling = NULL;
        }
        parent->last_child = child;
    } else {
        // Root window
        child->prev_sibling = NULL;
    }
    child->next_sibling = NULL;
    child->first_child = NULL;
    child->last_child = NULL;
    child->parent = parent;
}

void core_unlink_child(AwWindow* child) {
    AwWindow* parent = child->parent;
    AwWindow* next_child = child->next_sibling;
    if (parent->first_child == child) {
        if (parent->last_child == child) {
            parent->last_child = NULL;
        }
        parent->first_child = next_child;
        if (next_child) {
            next_child->prev_sibling = NULL;
        }
    } else {
        AwWindow* prev_child = child->prev_sibling;
        prev_child->next_sibling = next_child;
        next_child->prev_sibling = prev_child;
    }

    child->first_child = NULL;
    child->last_child = NULL;
    child->parent = NULL;
    child->next_sibling = NULL;
    child->prev_sibling = NULL;
}

void core_minimize_window(AwWindow* window) {
    if (!window->state.minimized) {
        if (window->state.maximized) {
            AwMinMaxWindow* mm = (AwMinMaxWindow*) window;
            window->window_rect = mm->save_window_rect;
            window->client_rect = mm->save_client_rect;
        } else {
            AwMinMaxWindow* mm = (AwMinMaxWindow*) window;
            mm->save_window_rect = window->window_rect;
            mm->save_client_rect = window->client_rect;
        }
        window->state.minimized = 1;
        window->state.maximized = 0;
        core_invalidate_window(window);
    }
}

void core_maximize_window(AwWindow* window) {
    if (!window->state.maximized) {
        if (window->state.minimized) {
            AwMinMaxWindow* mm = (AwMinMaxWindow*) window;
            window->window_rect = mm->save_window_rect;
            window->client_rect = mm->save_client_rect;
        } else {
            AwMinMaxWindow* mm = (AwMinMaxWindow*) window;
            mm->save_window_rect = window->window_rect;
            mm->save_client_rect = window->client_rect;
        }
        window->state.minimized = 0;
        window->state.maximized = 1;
        AwSize size = core_get_window_size(window->parent);
        core_move_window(window, 0, 0);
        core_resize_window(window, size.width, size.height);
    }
}

void core_restore_window(AwWindow* window) {
    if (window->state.minimized || window->state.maximized) {
        AwMinMaxWindow* mm = (AwMinMaxWindow*) window;
        window->window_rect = mm->save_window_rect;
        window->client_rect = mm->save_client_rect;
        window->state.minimized = 0;
        window->state.maximized = 0;
        core_invalidate_window(window);
    }
}

uint16_t get_next_context_id() {
    if (next_context_id <= AW_CONTEXT_ID_HIGH) {
        return next_context_id++;
    } else {
        return 0;
    }
}

uint16_t get_next_buffer_id() {
    if (next_buffer_id <= AW_BUFFER_ID_HIGH) {
        return next_buffer_id++;
    } else {
        return 0;
    }
}

uint16_t get_next_bitmap_id() {
    if (next_bitmap_id <= AW_BITMAP_ID_HIGH) {
        return next_bitmap_id++;
    } else {
        return 0;
    }
}

AwWindow* core_create_window(const AwCreateWindowParams* params) {
    AwCreateWindowParams prms;
    prms = *params;

    if ((prms.app == NULL) || (prms.wclass == NULL) || (prms.width < 0) || (prms.height < 0)) {
        return NULL; // bad parameter(s)
    }

    if (prms.style.sizeable && !prms.style.border) {
        return NULL; // pad parameter
    }

    if (prms.style.moveable && !prms.style.title_bar) {
        return NULL; // pad parameter
    }

    if (prms.parent == NULL) {
        prms.parent = root_window;
    }

    if (prms.context_id == AW_CONTEXT_ID_NEXT) {
        prms.context_id = get_next_context_id();
    }

    if (prms.buffer_id == AW_BUFFER_ID_NEXT) {
        prms.buffer_id = get_next_buffer_id();
    }

    if (prms.bitmap_id == AW_BITMAP_ID_NEXT) {
        prms.bitmap_id = get_next_bitmap_id();
    }

    // Allocate memory for the window structure
    size_t size = sizeof(AwWindow);
    if (prms.style.minimize_icon || prms.style.maximize_icon) {
        size = sizeof(AwMinMaxWindow);
    }
    AwWindow* window = (AwWindow*) malloc(size);
    if (window == NULL) {
        return NULL; // no memory
    }
    memset(window, 0, sizeof(AwWindow));

    if (prms.extra_data_size) {
        window->extra_data = malloc(prms.extra_data_size);
        if (!window->extra_data) {
            free(window);
            return NULL; // no memory
        }
    }

    if (prms.state.active && active_window) {
        core_activate_window(active_window, false);
    }

    vdp_context_select(0);
    vdp_logical_scr_dims(false);

    core_set_text(window, prms.text);

    window->style = prms.style;
    window->state = prms.state;
    window->app = prms.app;
    window->window_class = prms.wclass;
    window->window_rect.right = prms.width;
    window->window_rect.bottom = prms.height;
    window->bg_color = AW_DFLT_BG_COLOR;
    window->fg_color = AW_DFLT_FG_COLOR;

    core_link_child(prms.parent, window);

    AwMsg msg;
    msg.on_window_created.window = window;
    msg.on_window_created.msg_type = Aw_On_WindowCreated;
    core_post_message(&msg);

    msg.on_window_resized.msg_type = Aw_On_WindowResized;
    core_post_message(&msg);

    core_move_window(window, prms.x, prms.y);

    if (prms.state.active) {
        core_activate_window(window, true);
    }

    if (prms.state.maximized) {
        core_maximize_window(window);
    }

    return window;
}

void core_invalidate_window_rect(AwWindow* window, const AwRect* rect) {
    AwRect new_rect = *rect;
    core_offset_rect(&new_rect, window->window_rect.left, window->window_rect.top);
    AwRect extra_rect = core_get_intersect_rect(&window->window_rect, &new_rect);
    add_more_dirt(&extra_rect);
}

void core_invalidate_client_rect(AwWindow* window, const AwRect* rect) {
    AwRect new_rect = *rect;
    core_offset_rect(&new_rect, window->client_rect.left, window->client_rect.top);
    AwRect extra_rect = core_get_intersect_rect(&window->window_rect, &new_rect);
    add_more_dirt(&extra_rect);
}

void core_invalidate_title_bar(AwWindow* window) {
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

AwRect core_get_close_icon_rect(AwWindow* window) {
    if (window->style.close_icon) {
        int16_t thickness = get_border_thickness(window);
        AwRect rect = core_get_global_window_rect(window);
        rect.right -= thickness;
        rect.left = rect.right - AW_ICON_WIDTH;
        rect.top += thickness;
        rect.bottom = rect.top + AW_ICON_HEIGHT;
        return rect;
    } else {
        return core_get_empty_rect();
    }
}

AwRect core_get_minimize_icon_rect(AwWindow* window) {
    if (window->style.minimize_icon && !window->state.minimized) {
        int16_t thickness = get_border_thickness(window);
        AwRect rect = core_get_global_window_rect(window);
        rect.right -= thickness;
        if (window->style.close_icon) {
            rect.right -= AW_ICON_WIDTH;
        }
        rect.left = rect.right - AW_ICON_WIDTH;
        rect.top += thickness;
        rect.bottom = rect.top + AW_ICON_HEIGHT;
        return rect;
    } else {
        return core_get_empty_rect();
    }
}

AwRect core_get_maximize_icon_rect(AwWindow* window) {
    if (window->style.maximize_icon && !window->state.maximized) {
        int16_t thickness = get_border_thickness(window);
        AwRect rect = core_get_global_window_rect(window);
        rect.right -= thickness;
        if (window->style.close_icon) {
            rect.right -= AW_ICON_WIDTH;
        }
        if (window->style.minimize_icon && !window->state.minimized) {
            rect.right -= AW_ICON_WIDTH;
        }
        rect.left = rect.right - AW_ICON_WIDTH;
        rect.top += thickness;
        rect.bottom = rect.top + AW_ICON_HEIGHT;
        return rect;
    } else {
        return core_get_empty_rect();
    }
}

AwRect core_get_restore_icon_rect(AwWindow* window) {
    if ((window->style.minimize_icon || window->style.maximize_icon) &&
        (window->state.minimized || window->state.maximized)) {
        int16_t thickness = get_border_thickness(window);
        AwRect rect = core_get_global_window_rect(window);
        rect.right -= thickness;
        if (window->style.close_icon) {
            rect.right -= AW_ICON_WIDTH;
        }
        if ((window->style.minimize_icon && !window->state.minimized) ||
            (window->style.maximize_icon && !window->state.maximized)) {
            rect.right -= AW_ICON_WIDTH;
        }
        rect.left = rect.right - AW_ICON_WIDTH;
        rect.top += thickness;
        rect.bottom = rect.top + AW_ICON_HEIGHT;
        return rect;
    } else {
        return core_get_empty_rect();
    }
}

AwRect core_get_menu_icon_rect(AwWindow* window) {
    if (window->style.menu_icon) {
        int16_t thickness = get_border_thickness(window);
        AwRect rect = core_get_global_window_rect(window);
        rect.right -= thickness;
        if (window->style.close_icon) {
            rect.right -= AW_ICON_WIDTH;
        }
        if (window->style.minimize_icon || window->style.maximize_icon) {
            rect.right -= AW_ICON_WIDTH;
        }
        if (window->style.minimize_icon && window->style.maximize_icon) {
            rect.right -= AW_ICON_WIDTH;
        }
        rect.left = rect.right - AW_ICON_WIDTH;
        rect.top += thickness;
        rect.bottom = rect.top + AW_ICON_HEIGHT;
        return rect;
    } else {
        return core_get_empty_rect();
    }
}

void core_activate_window(AwWindow* window, bool active) {
    if (active) {
        if (window->state.enabled && (window != active_window)) {
            if (active_window) {
                core_activate_window(active_window, false);
            }
            window->state.active = 1;
            active_window = window;
            core_invalidate_window(window);
        }
    } else {
        if (window == active_window) {
            active_window = NULL;
            window->state.active = 0;
            core_invalidate_window(window);
        }
    }
}

void core_enable_window(AwWindow* window, bool enabled) {
    if (enabled) {
        if (!window->state.enabled) {
            window->state.enabled = 1;
            core_invalidate_window(window);
        }
    } else {
        if (window->state.enabled) {
            window->state.enabled = 0;
            if (window == active_window) {
                core_activate_window(active_window, false);
            }
            core_invalidate_window(window);
        }
    }
}

void core_show_window(AwWindow* window, bool visible) {
    if (visible) {
        if (!window->state.visible) {
            window->state.visible = 1;
            core_invalidate_window(window);
        }
    } else {
        if (window->state.visible) {
            window->state.visible = 0;
            core_invalidate_window(window);
        }
    }
}

void close_related_windows(AwWindow* window, AwApplication* app) {
    while (window) {
        close_related_windows(window->first_child, app);
        AwWindow* next_sibling = window->next_sibling;
        if (window->app == app) {
            window->style.primary = 0; // prevent issues
            core_close_window(window);
        }
        window = next_sibling;
    }
}

void core_close_window(AwWindow* window) {
    if (!window) {
        return;
    }

    uint8_t primary = window->style.primary;
    AwApplication* app = window->app;
    AwWindow* child = window->first_child;
    while (child) {
        child->style.primary = 0; // avoid issues
        AwWindow* next_sibling = child->next_sibling;
        core_close_window(child);
        child = next_sibling;
    }

    core_invalidate_window(window);
    core_activate_window(window, false);
    core_unlink_child(window);
    if (window->text_size) {
        free(window->text);
    }
    if (window->extra_data) {
        free(window->extra_data);
    }
    free(window);

    if (primary) {
        close_related_windows(root_window, app);
        core_unload_app(app);
    }
}

int32_t core_process_message(AwMsg* msg) {
    AwWindow* window = msg->do_common.window;
    bool halt = false;
    while (window) {
        const AwClass* wclass = window->window_class;
        while (wclass) {
            int32_t result = (*wclass->msg_handler)(window, msg, &halt);
            if (halt) {
                return result;
            }
            wclass = wclass->parent;
        }
        window = window->parent;
    }
    return 0;
}

void core_terminate() {
    running = false;
}

void core_unload_app(AwApplication* app) {
    for (uint8_t a = 0; a < AW_APP_LIST_SIZE; a++) {
        if (app_list[a].app == app) {
            AwLoadedApp* loaded_app = &app_list[a];
            (*loaded_app->hdr->stop)();
            loaded_app->app = NULL;
            loaded_app->hdr = NULL;
            app_count--;
            break;
        }
    }
}

void core_set_paint_msg(AwMsg* msg, AwWindow* window,
                bool entire, bool title, bool client) {
    AwDoMsgPaintWindow* paint_msg = &msg->do_paint_window;
    AwPaintFlags* paint_flags = &paint_msg->flags;

    paint_msg->window = window;
    paint_msg->msg_type = Aw_Do_PaintWindow;
    paint_msg->all_flags = 0;

    if (window->state.enabled) {
        paint_flags->enabled = 1;
    }

    if (window->state.selected) {
        paint_flags->selected = 1;
    }

    if (window->state.minimized) {
        paint_flags->window = 1;
        if (window->style.title_bar) {
            paint_flags->title_bar = 1;
            paint_flags->title = 1;
        }
        if (window->style.close_icon) {
            paint_flags->close_icon = 1;
        }
        if (window->style.maximize_icon) {
            paint_flags->maximize_icon = 1;
        }
        paint_flags->restore_icon = 1;
        return;
    } else {
        if (entire) {
            paint_flags->window = 1;
            if (window->style.border && !window->state.maximized) {
                paint_flags->border = 1;
            }
        }
    }
    
    if (entire || title) {
        if (window->style.title_bar) {
            paint_flags->title_bar = 1;
            paint_flags->title = 1;
        }
        if (window->style.close_icon) {
            paint_flags->close_icon = 1;
        }
        if (window->style.minimize_icon && !window->state.minimized) {
            paint_flags->minimize_icon = 1;
        }
        if (window->style.maximize_icon && !window->state.maximized) {
            paint_flags->maximize_icon = 1;
        }
        if ((window->style.minimize_icon || window->style.maximize_icon) &&
            (window->state.minimized || window->state.maximized)) {
            paint_flags->restore_icon = 1;
        }
        if (window->style.menu_icon) {
            paint_flags->menu_icon = 1;
        }
    }

    if (entire || client) {
        paint_flags->client = 1;
        paint_flags->background = 1;
        paint_flags->foreground = 1;
    }
}

void draw_background(AwWindow* window) {
    vdp_set_graphics_colour(0, window->bg_color | 0x80);
    vdp_clear_graphics();
}

void draw_foreground(AwWindow* window) {
    (void)window; // unused
}

uint8_t get_border_color(AwWindow* window) {
    return (window->state.active ?
            AW_DFLT_ACTIVE_BORDER_COLOR :
            AW_DFLT_INACTIVE_BORDER_COLOR);
}

uint8_t get_title_bar_color(AwWindow* window) {
    return (window->state.active ?
            AW_DFLT_ACTIVE_TITLE_BAR_COLOR:
            AW_DFLT_INACTIVE_TITLE_BAR_COLOR);
}

uint8_t get_title_color(AwWindow* window) {
    return (window->state.active ?
            AW_DFLT_ACTIVE_TITLE_COLOR :
            AW_DFLT_INACTIVE_TITLE_COLOR);
}

void draw_border(AwWindow* window) {
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
    int16_t thickness = get_border_thickness(window);
    vdp_set_graphics_colour(0, get_title_bar_color(window));
    AwSize size = core_get_client_size(window);

    vdp_move_to(thickness, thickness);
    vdp_filled_rect(thickness + size.width - 1,
                thickness + size.height - 1);
}

void draw_title(AwWindow* window) {
    int16_t thickness = get_border_thickness(window);
    vdp_set_graphics_colour(0, get_title_bar_color(window) | 0x80 );
    vdp_set_graphics_colour(0, get_title_color(window));
    vdp_move_to(thickness + 2, thickness + 2);
    vdp_write_at_graphics_cursor();
    printf("%s", window->text);
}

void core_set_client_viewport(AwWindow* window) {
    vdp_context_select(0);
    vdp_context_reset(0xFF); // all style set
    vdp_logical_scr_dims(false);
    vdp_move_to(window->client_rect.left, window->client_rect.top);
    vdp_move_to(window->client_rect.right-1, window->client_rect.bottom-1);
    local_vdp_set_graphics_viewport_via_plot();

    vdp_move_to(window->client_rect.left, window->client_rect.top);
    local_vdp_set_text_viewport_via_plot();
    local_vdp_set_graphics_origin_via_plot();
}

void core_set_title_viewport(AwWindow* window) {
    vdp_context_select(0);
    vdp_context_reset(0xFF); // all style set
    vdp_logical_scr_dims(false);
    AwRect rect = core_get_global_title_rect(window);
    vdp_move_to(rect.left, rect.top);
    vdp_move_to(rect.right-1, rect.bottom-1);
    local_vdp_set_graphics_viewport_via_plot();

    vdp_move_to(rect.left, rect.top);
    local_vdp_set_text_viewport_via_plot();
    local_vdp_set_graphics_origin_via_plot();
}

void core_set_window_viewport(AwWindow* window) {
    vdp_context_select(0);
    vdp_context_reset(0xFF); // all style set
    vdp_logical_scr_dims(false);
    vdp_move_to(window->window_rect.left, window->window_rect.top);
    vdp_move_to(window->window_rect.right-1, window->window_rect.bottom-1);
    local_vdp_set_graphics_viewport_via_plot();

    vdp_move_to(window->window_rect.left, window->window_rect.top);
    local_vdp_set_text_viewport_via_plot();
    local_vdp_set_graphics_origin_via_plot();
}

void core_paint_window(AwMsg* msg) {
    AwDoMsgPaintWindow* paint_msg = &msg->do_paint_window;
    AwWindow* window = paint_msg->window;

    if (!window->state.visible) {
        return;
    }

    AwPaintFlags* paint_flags = &paint_msg->flags;

    if (paint_flags->window) {
        if (window->state.minimized) {
            core_set_title_viewport(window);
        } else {
            core_set_window_viewport(window);
        }

        if (paint_flags->border) {
            draw_border(window);
        }
        if (paint_flags->title_bar) {
            draw_title_bar(window);
        }
        if (paint_flags->title) {
            draw_title(window);
        }
        if (paint_flags->close_icon) {
            AwRect rect = core_get_close_icon_rect(window);
            if (rect.right) {
                aw_draw_icon(AW_ICON_CLOSE, rect.left, rect.top);
            }
        }
        if (paint_flags->minimize_icon) {
            AwRect rect = core_get_minimize_icon_rect(window);
            if (rect.right) {
                aw_draw_icon(AW_ICON_MINIMIZE, rect.left, rect.top);
            }
        }
        if (paint_flags->maximize_icon) {
            AwRect rect = core_get_maximize_icon_rect(window);
            if (rect.right) {
                aw_draw_icon(AW_ICON_MAXIMIZE, rect.left, rect.top);
            }
        }
        if (paint_flags->restore_icon) {
            AwRect rect = core_get_restore_icon_rect(window);
            if (rect.right) {
                aw_draw_icon(AW_ICON_RESTORE, rect.left, rect.top);
            }
        }
        if (paint_flags->menu_icon) {
            AwRect rect = core_get_menu_icon_rect(window);
            if (rect.right) {
                aw_draw_icon(AW_ICON_MENU, rect.left, rect.top);
            }
        }
    }

    if (paint_flags->client) {
        core_set_client_viewport(window);

        if (paint_flags->background) {
            draw_background(window);
        }
        if (paint_flags->foreground) {
            draw_foreground(window);
        }
    }
}

int32_t on_mouse_dragged(AwMsg* msg) {
    (void)msg;
    return 0;
}

int32_t on_mouse_dropped(AwWindow* window, AwMsg* msg) {
    (void)window;
    AwRect rect = core_get_global_window_rect(msg->on_mouse_event.state.start_window);
    AwPoint pt;
    pt.x = rect.left;
    pt.y = rect.top;
    AwSize size;
    size.width = rect.right - rect.left;
    size.height = rect.bottom - rect.top;
    bool adjust_pos = false;
    bool adjust_size = false;

    switch (msg->on_mouse_event.state.target) {
        case AwMtLeftBorder: {
            pt.x = msg->on_mouse_event.state.cur_x;
            size.width = rect.right - pt.x;
            adjust_pos = true;
            adjust_size = true;
            break;
        }

        case AwMtTopBorder: {
            pt.y = msg->on_mouse_event.state.cur_y;
            size.height = rect.bottom - pt.y;
            adjust_pos = true;
            adjust_size = true;
            break;
        }

        case AwMtRightBorder: {
            size.width = msg->on_mouse_event.state.cur_x - rect.left;
            adjust_size = true;
            break;
        }

        case AwMtBottomBorder: {
            size.height = msg->on_mouse_event.state.cur_y - rect.top;
            adjust_size = true;
            break;
        }

        case AwMtUpperLeftCorner: {
            pt.x = msg->on_mouse_event.state.cur_x;
            size.width = rect.right - pt.x;
            pt.y = msg->on_mouse_event.state.cur_y;
            size.height = rect.bottom - pt.y;
            adjust_pos = true;
            adjust_size = true;
            break;
        }

        case AwMtUpperRightCorner: {
            size.width = msg->on_mouse_event.state.cur_x - rect.left;
            pt.y = msg->on_mouse_event.state.cur_y;
            size.height = rect.bottom - pt.y;
            adjust_pos = true;
            adjust_size = true;
            break;
        }

        case AwMtLowerRightCorner: {
            size.width = msg->on_mouse_event.state.cur_x - rect.left;
            size.height = msg->on_mouse_event.state.cur_y - rect.top;
            adjust_size = true;
            break;
        }

        case AwMtLowerLeftCorner: {
            pt.x = msg->on_mouse_event.state.cur_x;
            size.width = rect.right - pt.x;
            size.height = msg->on_mouse_event.state.cur_y - rect.top;
            adjust_pos = true;
            adjust_size = true;
            break;
        }

        case AwMtTitleBar: {
            pt.x += msg->on_mouse_event.state.cur_x - msg->on_mouse_event.state.start_x;
            pt.y += msg->on_mouse_event.state.cur_y - msg->on_mouse_event.state.start_y;
            adjust_pos = true;
            break;
        }

        default: {
        }
    }

    if (adjust_pos) {
        AwRect parent_rect;
        if (msg->on_mouse_event.state.start_window->parent) {
            parent_rect = core_get_global_client_rect(msg->on_mouse_event.state.start_window->parent);
        } else {
            parent_rect = core_get_screen_rect();
        }

        AwMsg new_msg;
        new_msg.do_move_window.window = msg->on_mouse_event.state.start_window;
        new_msg.do_move_window.msg_type = Aw_Do_MoveWindow;
        new_msg.do_move_window.pt.x = pt.x - parent_rect.left;
        new_msg.do_move_window.pt.y = pt.y - parent_rect.top;
        core_post_message(&new_msg);
    }

    if (adjust_size) {
        AwMsg new_msg;
        new_msg.do_resize_window.window = msg->on_mouse_event.state.start_window;
        new_msg.do_resize_window.msg_type = Aw_Do_ResizeWindow;
        new_msg.do_resize_window.width = size.width;
        new_msg.do_resize_window.height = size.height;
        core_post_message(&new_msg);
    }

    return 0;
}

int32_t core_handle_message(AwWindow* window, AwMsg* msg, bool* halt) {
    *halt = true; // this is the last chance to handle a messge
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

        case Aw_Do_UnloadApp: {
            core_unload_app(msg->do_unload_app.app);
            break;
        }

        default: {
            switch (msg->on_common.msg_type) {

                case Aw_On_Common: {
                    break;
                }

                case Aw_On_KeyEvent: {
                    break;
                }

                case Aw_On_LeftButtonDown: {
                    break;
                }

                case Aw_On_LeftButtonUp: {
                    break;
                }

                case Aw_On_LeftButtonClick: {
                    switch (msg->on_mouse_event.state.target) {
                        case AwMtCloseIcon: {
                            core_close_window(msg->on_mouse_event.window);
                            break;
                        }
                        case AwMtMinimizeIcon: {
                            core_minimize_window(msg->on_mouse_event.window);
                            break;
                        }
                        case AwMtMaximizeIcon: {
                            core_maximize_window(msg->on_mouse_event.window);
                            break;
                        }
                        case AwMtRestoreIcon: {
                            core_restore_window(msg->on_mouse_event.window);
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    break;
                }

                case Aw_On_LeftButtonLongClick: {
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

                case Aw_On_MiddleButtonLongClick: {
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

                case Aw_On_RightButtonLongClick: {
                    break;
                }

                case Aw_On_RightButtonDoubleClick: {
                    break;
                }

                case Aw_On_MouseMoved: {
                    break;
                }

                case Aw_On_MouseDragged: {
                    return on_mouse_dragged(msg);
                }

                case Aw_On_MouseDropped: {
                    return on_mouse_dropped(window, msg);
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
                    break;
                }
            
                case Aw_On_Terminate: {
                    break;
                }

                case Aw_On_RealTimeClockEvent: {
                    break;
                }

                default: {
                    break;
                }
            }
        }
    }

    return 0;
}

void core_initialize() {
    AwCreateWindowParams params;
    memset(&params, 0, sizeof(params));
    params.app = &agwin_app;
    params.parent = NULL;
    params.wclass = &root_class;
    params.style.border = 1;
    params.state.enabled = 1;
    params.state.visible = 1;
    params.width = AW_SCREEN_WIDTH;
    params.height = AW_SCREEN_HEIGHT;
    params.text = "Agon Windows Root";

    root_window = core_create_window(&params);

    running = true;
}

bool paint_windows(AwWindow* window, AwRect* painted, bool can_paint) {
    if (!window->state.visible) {
        return false;
    }

    // See whether the dirty area becomes covered by this window's siblings
    if (window->next_sibling) {
        if (paint_windows(window->next_sibling, painted, false)) {
            return true;
        }
    }

    // See whether the dirty area becomes covered by this window's children
    bool covered_by_kids = false;
    if (!window->state.minimized) {
        AwRect rect = core_get_empty_rect();
        AwWindow* child = window->first_child;
        while (child) {
            if (paint_windows(child, &rect, false)) {
                covered_by_kids = true;
                break;
            }
            if (rect.left == dirty_area.left &&
                rect.top == dirty_area.top &&
                rect.right == dirty_area.right &&
                rect.bottom == dirty_area.bottom) {
                covered_by_kids = true;
                break;
            }
            child = child->next_sibling;
        }
    }

    // Determine the dirty area for this window, if any
    AwMsg msg;
    msg.do_paint_window.paint_rect = core_get_empty_rect();

    AwRect title_rect = core_get_global_title_rect(window);
    AwRect dirty_title_rect = core_get_intersect_rect(&dirty_area, &title_rect);
    AwSize dirty_title_size = core_get_rect_size(&dirty_title_rect);

    AwRect client_rect = core_get_global_client_rect(window);
    AwRect dirty_client_rect = core_get_intersect_rect(&dirty_area, &client_rect);
    AwSize dirty_client_size = core_get_rect_size(&dirty_client_rect);

    AwRect window_rect = core_get_intersect_rect(&dirty_area, &window_rect);
    AwRect dirty_window_rect = core_get_intersect_rect(&dirty_area, &window_rect);
    AwSize dirty_window_size = core_get_rect_size(&dirty_window_rect);

    if (!covered_by_kids) {
        bool any = true;
        if (dirty_window_size.width || (dirty_title_size.width && dirty_client_size.width)) {
            msg.do_paint_window.paint_rect = dirty_window_rect;
            if (can_paint) {
                core_set_paint_msg(&msg, window, true, false, false);
                core_process_message(&msg);
            }
        } else if (dirty_title_size.width) {
            msg.do_paint_window.paint_rect = dirty_title_rect;
            if (can_paint) {
                core_set_paint_msg(&msg, window, false, true, false);
                core_process_message(&msg);
            }
        } else if (dirty_client_size.width) {
            msg.do_paint_window.paint_rect = dirty_client_rect;
            if (can_paint) {
                core_set_paint_msg(&msg, window, false, false, true);
                core_process_message(&msg);
            }
        } else {
            any = false;
        }

        if (any) {
            // Add to the painted (covered) area
            AwRect rect = core_get_union_rect(painted, &msg.do_paint_window.paint_rect);
            *painted = rect;
        }
    }

    // Paint this window's children
    if (can_paint && !window->state.minimized) {
        AwWindow* child = window->first_child;
        while (child) {
            if (paint_windows(child, painted, true)) {
                covered_by_kids = true;
            }
            child = child->next_sibling;
        }
    }
    return covered_by_kids;
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
                AwRect painted = core_get_empty_rect();
                paint_windows(root_window, &painted, true);
                dirty_area = core_get_empty_rect();
            } else {
                update_rtc_state();
                uint32_t now = (uint32_t) clock();
                time_t ticks = now - last_rtc_request;
                if (ticks >= RTC_CHECK_TICKS) {
                    last_rtc_request = now;
                    vdp_request_rtc(0);
                }
                vdp_update_key_state();
                update_mouse_state();
            }
        }
    }
    vdp_key_reset_interrupt();
}

const AwFcnTable aw_core_functions = {
    core_activate_window,
    core_close_window,
    core_create_window,
    core_enable_window,
    core_expand_rect,
    core_expand_rect_height,
    core_expand_rect_width,
    core_expand_rect_unevenly,
    core_free,
    core_get_active_window,
    core_get_client_size,
    core_get_close_icon_rect,
    core_get_empty_rect,
    core_get_global_client_rect,
    core_get_global_title_rect,
    core_get_global_window_rect,
    core_get_intersect_rect,
    core_get_local_client_rect,
    core_get_local_window_rect,
    core_get_maximize_icon_rect,
    core_get_menu_icon_rect,
    core_get_minimize_icon_rect,
    core_get_rect_height,
    core_get_rect_size,
    core_get_rect_width,
    core_get_restore_icon_rect,
    core_get_root_class,
    core_get_root_window,
    core_get_screen_center,
    core_get_screen_rect,
    core_get_screen_size,
    core_get_sizing_client_rect,
    core_get_sizing_window_rect,
    core_get_top_level_window,
    core_get_union_rect,
    core_get_version,
    core_get_window_size,
    core_handle_message,
    core_initialize,
    core_invalidate_client,
    core_invalidate_client_rect,
    core_invalidate_title_bar,
    core_invalidate_window,
    core_invalidate_window_rect,
    core_load_app,
    core_malloc,
    core_message_loop,
    core_move_window,
    core_offset_rect,
    core_paint_window,
    core_post_message,
    core_process_message,
    core_realloc,
    core_rect_contains_point,
    core_resize_window,
    core_set_client_viewport,
    core_set_text,
    core_set_title_viewport,
    core_set_window_viewport,
    core_show_window,
    core_terminate,
    core_unload_app
};

int core_load_app(const char* path) {
    AwAppHeader app_header;
	FILE *fp;
    int rc = -1;
    if (app_count >= AW_APP_LIST_SIZE) {
        return rc;
    }

    if ((fp = fopen(path, "rb"))) {
        size_t byte_cnt = fread((void*)&app_header, 1, sizeof(app_header), fp);
        if ((byte_cnt == (size_t) sizeof(app_header)) &&
            app_header.marker[0] == 'W' &&
            app_header.marker[1] == 'I' &&
            app_header.marker[2] == 'N') {
            fseek(fp, 0, SEEK_END);
            long file_size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            byte_cnt = fread(app_header.load_address, 1, file_size, fp);
            if (byte_cnt == file_size) {
                AwAppHeader* hdr = (AwAppHeader*) app_header.load_address;
                hdr->core_functions = &aw_core_functions;
                AwStart start = (AwStart) app_header.load_address;
                for (uint8_t a = 0; a < AW_APP_LIST_SIZE; a++) {
                    if (!app_list[a].app) {
                        AwLoadedApp* loaded_app = &app_list[a];
                        loaded_app->app = hdr->app;
                        loaded_app->hdr = hdr;
                        app_count++;
                        break;
                    }
                }
                rc = (*start)();
            }
        }
        fclose(fp);
    }
    return rc;
}

void* core_malloc(size_t size) {
    return malloc(size);
}

void* core_realloc(void* ptr, size_t size) {
    return realloc(ptr, size); 
}

void core_free(void* ptr) {
    free(ptr);
}

#ifdef __CPLUSPLUS
} // extern "C"
#endif
