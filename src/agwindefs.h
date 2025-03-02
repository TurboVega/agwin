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

#pragma once

#include <stdint.h>
#include <stdbool.h>

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

#ifndef NULL
#define NULL 0
#endif

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

typedef struct tag_AwApplication AwApplication;
typedef struct tag_AwClass AwClass;
typedef struct tag_AwWindow AwWindow;
typedef union tag_AwMsg AwMsg;

// The window may be the same window as specified in the message,
// or may be an ancestor window. The return value may depend on
// the message type (primarily for use with immediate processing,
// as opposed to posting). To halt further processing of a message,
// set the outgoing halt value to true.
//
typedef int32_t (*AwMsgHandler)(AwWindow* window, AwMsg* msg, bool* halt);

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

typedef struct tag_AwWindowStyle {
    uint16_t        top_level : 1;  // whether the window is a top-level window
    uint16_t        popup : 1;      // whether the window is a popup (e.g., dialog) window
    uint16_t        border : 1;     // whether the window has a border
    uint16_t        title_bar : 1;  // whether the window has a title bar
    uint16_t        close_icon: 1;  // whether the window has the close icon in the title bar
    uint16_t        minimize_icon: 1;  // whether the window has the minimize icon in the title bar
    uint16_t        maximize_icon: 1;  // whether the window has the maximize icon in the title bar
    uint16_t        menu_icon: 1;   // whether the window has the menu icon in the title bar
    uint16_t        sizeable : 1;   // whether the window can be resized
    uint16_t        moveable : 1;   // whether the window can be moved
    uint16_t        reserved : 6;   // reserved
} AwWindowStyle;

typedef struct tag_AwWindowState {
    uint16_t        active : 1;     // whether the window is active
    uint16_t        enabled : 1;    // whether the window is enabled
    uint16_t        selected : 1;   // whether the window is selected
    uint16_t        visible : 1;    // whether the window is visible
    uint16_t        minimized : 1;  // whether the window is minimized
    uint16_t        maximized : 1;  // whether the window is maximized
    uint16_t        title_dirty : 1;  // whether the title bar is dirty
    uint16_t        client_dirty : 1; // whether the client area is dirty
    uint16_t        window_dirty : 1; // whether the entire window is dirty
    uint16_t        reserved : 7;   // reserved
} AwWindowState;

typedef struct tag_AwPaintFlags {
    uint16_t        border : 1;     // paint the border
    uint16_t        title_bar : 1;  // paint the title bar
    uint16_t        title : 1;      // paint the title
    uint16_t        close_icon: 1;  // paint the close icon in the title bar
    uint16_t        minimize_icon: 1;  // paint the minimize icon in the title bar
    uint16_t        maximize_icon: 1;  // paint the maximize icon in the title bar
    uint16_t        restore_icon: 1;   // paint the restore icon in the title bar
    uint16_t        menu_icon: 1;   // paint the menu icon in the title bar
    uint16_t        background : 1; // paint the client background
    uint16_t        foreground : 1; // paint the client foreground
    uint16_t        enabled : 1;    // whether the window is enabled
    uint16_t        selected : 1;   // whether the window is selected
    uint16_t        window : 1;     // whether painting outer window decorations
    uint16_t        client : 1;     // whether painting inner client area
    uint16_t        reserved1 : 1;  // reserved
    uint16_t        reserved2 : 1;  // reserved
    uint16_t        reserved3 : 1;  // reserved
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

// This comes from MOS system variables via AgDev SYSVAR
// The "start_*" members only apply for drag/drop messages
typedef struct tag_AwMouseState {
    AwWindow*   start_window;       // window under mouse when button first pressed
	uint16_t    start_x;            // X coordinate of the mouse cursor when button first pressed
	uint16_t    start_y;            // Y coordinate of the mouse cursor when button first pressed
	uint16_t    cur_x;              // current X coordinate of the mouse cursor
	uint16_t    cur_y;              // current Y coordinate of the mouse cursor
    union {
    	uint8_t     buttons;        // All button indicators
        struct {
            uint8_t left;           // whether the LEFT button is pressed
            uint8_t middle;         // whether the MIDDLE button is pressed
            uint8_t right;          // whether the RIGHT button is pressed
        };
    };
	uint8_t     wheel;              // wheel rotation
	uint16_t    delta_x;            // amount X changed
	uint16_t    delta_y;            // amount Y changed
} AwMouseState;

typedef struct tag_AwApplication {
    const char*     name;           // name of the app
    uint32_t*       load_address;   // location that the app was loaded
    uint32_t        memory_size;    // amount of memory allocated to app
    const AwClass*  classes;        // points to array of window classes (may be NULL)
    AwWindow*       first_windows;  // points to chain of windows created by/for the app
    uint8_t         num_classes;    // number of window classes defined by the app
} AwApplication;

typedef struct tag_AwClass {
    const char*     name;           // name of the class
    const AwClass*  parent;         // points to the parent class (may be NULL)
    AwMsgHandler    msg_handler;    // points to the message handler function
} AwClass;

typedef struct tag_AwWindow {
    AwApplication*  app;            // points to the app that owns the window
    const AwClass*  window_class;   // points to the class of the app
    AwWindow*       parent;         // points to the parent window
    AwWindow*       first_child;    // points to the first child window
    AwWindow*       last_child;     // points to the last child window
    AwWindow*       prev_sibling;   // points to the previous sibling window
    AwWindow*       next_sibling;   // points to the next sibling window
    char*           text;           // title of the window or text content
    AwRect          window_rect;    // rectangle enclosing the entire window (relative to screen)
    AwRect          client_rect;    // rectangle enclosing the client area (relative to screen)
    AwWindowStyle   style;          // indicators describing the style of the window
    AwWindowState   state;          // indicators describing the current state of the window
    uint32_t        text_size;      // allocated space for text (not the text length)
    void*           extra_data;     // additional custom data defined by the app
    uint8_t         bg_color;       // color of background, espcially when drawn by core
    uint8_t         fg_color;       // color of text, espcially when drawn by core
} AwWindow;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
