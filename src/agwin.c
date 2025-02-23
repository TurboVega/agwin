#include <agon/vdp_vdu.h>
#include <agon/vdp_key.h>
#include <stdio.h>
#include <mos_api.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "agwin.h"

void key_event_handler( KEY_EVENT key_event )
{
	if ( key_event.code == 0x7d ) {
		vdp_cursor_enable( true );
		exit( 1 );						// Exit program if esc pressed
	}
	vdp_cursor_tab( 0, 0 );
//	printf( "Modifier %02x, key-code %02x, up/down %02x\n",
//			key_event.mods, key_event.code, key_event.down );
	for ( int i = 31; i >= 0; i-- ) printf( "%02x", vdp_key_bits[i] );
	return;
}

void wait_clock(clock_t ticks)
{
	clock_t ticks_now = clock();

	do {
		vdp_update_key_state();
	} while (clock() - ticks_now < ticks);
}

void game_loop()
{
    srand(clock());

    while (true) {
        vdp_swap();
    };
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
	vdp_set_key_event_handler( key_event_handler );
	game_loop();
	vdp_cursor_enable( true );
	printf( "Press any key to continue...\n" );
	getchar();
	return 0;
}
