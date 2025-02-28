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
	core_message_loop();

	vdp_context_select(0);
    vdp_context_restore();

	vdp_cursor_enable(true);
	vdp_mouse_disable();
	return 0;
}
