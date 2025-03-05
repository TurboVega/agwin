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

#include <agon/vdp_vdu.h>
#include <agon/vdp_key.h>
#include <stdio.h>
#include <mos_api.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "agwincore.h"
#include "agwinicon.h"

extern volatile SYSVAR * sys_var; // points to MOS system variables

// The "my..." code and data is used for a simple test, and will
// be removed once there is a better example of a loaded app.

int32_t my_handle_message(AwWindow* window, AwMsg* msg, bool* halt);

AwClass my_class = { "myclass", NULL, my_handle_message };

AwApplication my_app = { "myapp", 0, 0, &my_class, NULL, 1 };

void init_my_app() {
    my_class.parent = core_get_root_class();

    AwWindowStyle style;
    style.border = 1;
    style.title_bar = 1;
    style.close_icon = 1;
    style.minimize_icon = 1;
    style.maximize_icon = 1;
    style.menu_icon = 1;
    style.sizeable = 1;
    style.moveable = 1;

    AwWindowState state;
    state.active = 0;
    state.enabled = 1;
    state.selected = 0;
    state.visible = 1;

    for (uint16_t row = 2; row < 3; row++) {
        uint16_t y = row * 116 + 5; // 5, 121, 237, 348
        for (uint16_t col = 2; col < 3; col++) {
            uint16_t x = col * 155 + 5; // 5, 160, 315, 465
            uint16_t i = row * 4 + col;
            char text[10];
            sprintf(text, "Color #%02hu", i);
            printf("%s\r\n", text);
            AwWindow* win = core_create_window(&my_app, NULL,
								&my_class, style, state,
                                x, y, 150, 112, text, 0);
            win->bg_color = i;
            win->fg_color = 15 - i;
            if (i == 10) {
                core_activate_window(win, true);
            }
        }
    }

    core_load_app("winclock.bin");
}

int32_t my_handle_message(AwWindow* window, AwMsg* msg, bool* halt) {
    (void)window; // presently unused
    (void)msg; // presently unused
    (void)halt; // presently unused
	return 0; // default to core processing
}

int main( void )
{
	sys_var = vdp_vdu_init();
	if (vdp_key_init() == -1) return 1;

	vdp_mode(AW_SCREEN_MODE);
	vdp_logical_scr_dims(false);
	vdp_clear_screen();
    vdp_clear_graphics();

	vdp_context_select(0);
    vdp_context_save();

	vdp_cursor_enable(false);
	vdp_mouse_reset();
	vdp_mouse_enable();
    vdp_mouse_set_cursor(AwMcPointerSimple);

	AwPoint center = core_get_screen_center();
	vdp_mouse_set_position(center.x, center.y);

    core_initialize();
    aw_register_icons();
	init_my_app();
	core_message_loop();

	vdp_context_select(0);
    vdp_context_restore();

	vdp_cursor_enable(true);
	vdp_mouse_disable();
	return 0;
}
