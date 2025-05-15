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
    AwCreateWindowParams params;

    my_class.parent = core_get_root_class();

    for (uint16_t row = 0; row < 1; row++) {
        uint16_t y = row * 116 + 5; // 5, 121, 237, 348
        for (uint16_t col = 0; col < 1; col++) {
            uint16_t x = col * 155 + 5; // 5, 160, 315, 465
            uint16_t i = (AW_SCREEN_COLORS == 64 ? (row * 13 + col * 3) : (row * 4 + col));
            char text[10];
            sprintf(text, "Color #%02hu", i);

            memset(&params, 0, sizeof(params));
            params.app = &my_app;
            params.wclass = &my_class;
            params.style.border = 1;
            params.style.title_bar = 1;
            params.style.close_icon = 1;
            params.style.minimize_icon = 1;
            params.style.maximize_icon = 1;
            params.style.menu_icon = 1;
            params.style.sizeable = 1;
            params.style.moveable = 1;
            params.style.primary = 1;
            params.state.enabled = 1;
            params.state.visible = 1;
            params.x = x;
            params.y = y;
            params.width = 148;
            params.height = 112;
            params.text = text;

            AwWindow* win = core_create_window(&params);

            win->bg_color = i;
            win->fg_color = 15 - i;
            if (i == 10) {
                core_activate_window(win, true);
            }
        }
    }

    core_load_app("winclock.bin");

    memset(&params, 0, sizeof(params));
    params.app = &my_app;
    params.wclass = &my_class;
    params.style.border = 1;
    params.style.title_bar = 1;
    params.style.close_icon = 1;
    params.style.minimize_icon = 1;
    params.style.maximize_icon = 1;
    params.style.menu_icon = 1;
    params.style.sizeable = 1;
    params.style.moveable = 1;
    params.style.primary = 1;
    params.state.enabled = 1;
    params.state.visible = 1;
    params.x = 99;
    params.y = 99;
    params.width = 148;
    params.height = 112;
    params.text = "I'm Over All!";

    AwWindow* win = core_create_window(&params);

    win->bg_color = 14;
    win->fg_color = 0;
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

    if (AW_SCREEN_COLORS < 64) {
        core_create_palette_buffer(AW_PALETTE_BUFFER, AW_SCREEN_COLORS);
    }

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
