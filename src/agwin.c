#include <agon/vdp_vdu.h>
#include <agon/vdp_key.h>
#include <stdio.h>
#include <mos_api.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "agwincore.h"

extern volatile SYSVAR * sys_var; // points to MOS system variables

/*
void wait_clock(clock_t ticks)
{
	clock_t ticks_now = clock();

	do {
		vdp_update_key_state();
	} while (clock() - ticks_now < ticks);
}
*/

int32_t my_handle_message(AwWindow* window, AwMsg* msg, bool* halt);

AwClass my_class = { "myclass", NULL, my_handle_message };

AwApplication my_app = { "myapp", 0, 0, &my_class, NULL, 1 };

void init_my_app() {
    my_class.parent = core_get_root_class();

    AwWindowFlags flags;
    flags.border = 1;
    flags.title_bar = 1;
    flags.icons = 1;
    flags.sizeable = 0;
    flags.active = 0;
    flags.enabled = 1;
    flags.selected = 0;
    flags.visible = 1;

    for (uint16_t row = 0; row < 4; row++) {
        uint16_t y = row * 116 + 5; // 5, 121, 237, 348
        for (uint16_t col = 0; col < 4; col++) {
            uint16_t x = col * 155 + 5; // 5, 160, 315, 465
            uint16_t i = row * 4 + col;
            char text[10];
            sprintf(text, "Color #%02hu", i);
            printf("%s\r\n", text);
            AwWindow* win = core_create_window(&my_app, NULL,
								&my_class, flags,
                                x, y, 150, 112, text, 0);
            win->bg_color = i;
            win->fg_color = 15 - i;
            if (i == 10) {
                core_activate_window(win, true);
            }
        }
    }
}

int32_t my_handle_message(AwWindow* window, AwMsg* msg, bool* halt) {
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

	AwPoint center = core_get_screen_center();
	vdp_mouse_set_position(center.x, center.y);

    core_initialize();
	init_my_app();
	core_message_loop();

	vdp_context_select(0);
    vdp_context_restore();

	vdp_cursor_enable(true);
	vdp_mouse_disable();
	return 0;
}
