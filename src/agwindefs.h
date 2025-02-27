#pragma once

#include <stdint.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

#pragma pack(push, 1)

#define AW_MAJOR        0
#define AW_MINOR        1
#define AW_VERSION      ((AW_MAJOR << 4) | AW_MINOR)

#define AW_CONTEXT_ID_LOW       0x8000  // lowest VDP context ID used by agwin
#define AW_CONTEXT_ID_HIGH      0x8FFF  // highest VDP context ID used by agwin

#define AW_BORDER_THICKNESS     2       // pixels
#define AW_TITLE_BAR_HEIGHT     12      // pixels
#define AW_TTITLE_TEXT_HEIGHT   8       // pixels
#define AW_SCREEN_MODE          0       // 640x480x60Hz, 16 colors

// Some standard window classes
#define AW_CLASS_ROOT           ((uint16_t) 1)
#define AW_CLASS_MENU           ((uint16_t) 2)
#define AW_CLASS_MENU_ITEM      ((uint16_t) 3)
#define AW_CLASS_LIST           ((uint16_t) 4)
#define AW_CLASS_LIST_ITEM      ((uint16_t) 5)
#define AW_CLASS_EDIT_TEXT      ((uint16_t) 6)
#define AW_CLASS_STATIC_TEXT    ((uint16_t) 7)
#define AW_CLASS_MESSAGE_BOX    ((uint16_t) 8)
#define AW_CLASS_ICON           ((uint16_t) 9)
#define AW_CLASS_USER           ((uint16_t) 100) // and onward

#ifndef NULL
#define NULL 0
#endif

#define AW_MSG_UNHANDLED        0
#define AW_MSG_HANDLED          1

#define AW_DFLT_BG_COLOR                    15
#define AW_DFLT_FG_COLOR                    0
#define AW_DFLT_ACTIVE_BORDER_COLOR         12
#define AW_DFLT_ACTIVE_TITLE_BAR_COLOR      4
#define AW_DFLT_ACTIVE_TITLE_COLOR          11
#define AW_DFLT_INACTIVE_BORDER_COLOR       8
#define AW_DFLT_INACTIVE_TITLE_BAR_COLOR    7
#define AW_DFLT_INACTIVE_TITLE_COLOR        0

#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) > (b) ? (a) : (b))

typedef union tag_AwMsg AwMsg;
typedef struct tag_AwWindow AwWindow;

typedef int32_t (*AwMsgHandler)(AwMsg* msg);

typedef struct tag_AwRect {
    int16_t     left;       // inclusive left edge of the rectangle (this is inside the rectangle)
    int16_t     top;        // inclusive top edge of the rectangle (this is inside the rectangle)
    int16_t     right;      // exclusive right edge of the rectangle (this is outside the rectangle)
    int16_t     bottom;     // exclusive bottom edge of the rectangle (this is outside the rectangle)
} AwRect;

typedef struct tag_AwPoint {
    int16_t     x;          // X coordinate
    int16_t     y;          // Y coordinate
} AwPoint;

typedef struct tag_AwSize {
    int16_t     width;      // horizontal size
    int16_t     height;     // vertical size
} AwSize;

typedef struct tag_AwWindowFlags {
    uint16_t        top_level : 1;  // whether the window is a top-level window
    uint16_t        popup : 1;      // whether the window is a popup (e.g., dialog) window
    uint16_t        border : 1;     // whether the window has a border
    uint16_t        title_bar : 1;  // whether the window has a title bar
    uint16_t        icons : 1;      // whether the window has icons in the title bar
    uint16_t        sizeable : 1;   // whether the window can be resized
    uint16_t        active : 1;     // whether the window is active
    uint16_t        enabled : 1;    // whether the window is enabled
    uint16_t        selected : 1;   // whether the window is selected
    uint16_t        visible : 1;    // whether the window is visible
    uint16_t        title_dirty : 1;  // reserved
    uint16_t        client_dirty : 1; // reserved
    uint16_t        window_dirty : 1; // reserved
    uint16_t        reserved4 : 1;  // reserved
    uint16_t        reserved5 : 1;  // reserved
    uint16_t        reserved6 : 1;  // reserved
} AwWindowFlags;

typedef struct tag_AwPaintFlags {
    uint16_t        border : 1;     // paint the border
    uint16_t        title_bar : 1;  // paint the title bar
    uint16_t        title : 1;      // paint the title
    uint16_t        icons: 1;       // paint the icons in the title bar
    uint16_t        background : 1; // paint the client background
    uint16_t        foreground : 1; // paint the client foreground
    uint16_t        enabled : 1;    // whether the window is enabled
    uint16_t        selected : 1;   // whether the window is selected
    uint16_t        window : 1;     // whether painting outer window decorations
    uint16_t        client : 1;     // whether painting inner client area
    uint16_t        reserved1 : 1;  // reserved
    uint16_t        reserved2 : 1;  // reserved
    uint16_t        reserved3 : 1;  // reserved
    uint16_t        reserved4 : 1;  // reserved
    uint16_t        reserved5 : 1;  // reserved
    uint16_t        reserved6 : 1;  // reserved
} AwPaintFlags;

// This is more-or-less a copy of the AgDev KEY_EVENT
typedef union tag_AwKeyState {
    uint32_t        key_data;       // raw key data
    struct {
        union {
            uint8_t all_mods;       // all modifiers
            struct {
                uint8_t ctrl : 1;           // whether the CTRL key is currently pressed
                uint8_t shift : 1;          // whether the SHIFT key is currently pressed
                uint8_t left_alt : 1;       // whether the LEFT ALT key is currently pressed
                uint8_t right_alt : 1;      // whether the RIGHT ALT key is currently pressed
                uint8_t caps_lock : 1;      // whether the CAPS LOCK is currently active
                uint8_t num_lock : 1;       // whether the NUM LOCK is currently active
                uint8_t scroll_lock : 1;    // whether the SCROLL LOCK is currently active
                uint8_t gui : 1;            // GUI flag bit from MOS
            } modifiers;
        };
        uint8_t     ascii_code;     // ASCII code for the key
        uint8_t     key_code;       // keyboard code for the key
        uint8_t     down;           // whether the key is currently pressed
    };
} AwKeyState;

typedef struct tag_AwApplication {
    const char*     name;           // name of the app
    AwMsgHandler    msg_handler;    // points to the message handler function
    uint32_t*       load_address;   // location that the app was loaded
    uint32_t        memory_size;    // amount of memory allocated to app
} AwApplication;

typedef struct tag_AwWindow {
    AwApplication*  app;            // points to the app that owns the window
    AwWindow*       parent;         // points to the parent window
    AwWindow*       first_child;    // points to the first child window
    AwWindow*       last_child;     // points to the last child window
    AwWindow*       prev_sibling;   // points to the previous sibling window
    AwWindow*       next_sibling;   // points to the next sibling window
    char*           text;           // title of the window or text content
    uint16_t        class_id;       // non-unique class ID for the window
    AwRect          window_rect;    // rectangle enclosing the entire window (relative to screen)
    AwRect          client_rect;    // rectangle enclosing the client area (relative to screen)
    AwWindowFlags   flags;          // flags describing the window
    uint32_t        text_size;      // allocated space for text (not the text length)
    uint8_t         bg_color;       // color of background, espcially when drawn by core
    uint8_t         fg_color;       // color of text, espcially when drawn by core
} AwWindow;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
