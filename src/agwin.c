#include <agon/vdp_vdu.h>
#include <stdio.h>
#include <mos_api.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "agwin.h"

void aw_initialize();
void aw_message_loop();

void wait_clock(clock_t ticks)
{
	clock_t ticks_now = clock();

	do {
		vdp_update_key_state();
	} while (clock() - ticks_now < ticks);
}

int main( void )
{
	vdp_vdu_init();
	if (vdp_key_init() == -1) return 1;

	vdp_mode(SCREEN_MODE);
	vdp_clear_screen();
    vdp_clear_graphics();
	vdp_logical_scr_dims(false);
	vdp_cursor_enable(false);
	vdp_set_key_event_handler(key_event_handler);
    aw_initialize();
	aw_message_loop();
	vdp_cursor_enable(true);
	return 0;
}
