#pragma once

#include <stdint.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

// Some standard window classes
#define AW_CLASS_DESKTOP        1
#define AW_CLASS_MENU           2
#define AW_CLASS_MENU_ITEM      3
#define AW_CLASS_LIST           4
#define AW_CLASS_LIST_ITEM      5
#define AW_CLASS_EDIT_TEXT      6
#define AW_CLASS_STATIC_TEXT    7
#define AW_CLASS_MESSAGE_BOX    8

#define AW_CONTEXT_ID_LOW       0x8000  // lowest VDP context ID used by agwin
#define AW_CONTEXT_ID_HIGH      0x8FFF  // highest VDP context ID used by agwin

#define AW_BORDER_THICKNESS     4       // pixels
#define AW_TITLE_BAR_HEIGHT     12      // pixels
#define AW_TTITLE_TEXT_HEIGHT   8       // pixels
#define AW_SCREEN_MODE          0       // 640x480x60Hz, 16 colors

#ifdef __CPLUSPLUS
} // extern "C"
#endif
