#pragma once

#include <stdint.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

#pragma pack(push, 1)

typedef enum uint8_t {
    PaintWindow,
    PaintBorder,
    PaintTitleBar,
    PaintTitle,
    PaintIcons,
    PaintClient,
    PaintForeground,
    PaintBackground,
    KeyAction,
    KeyDown,
    KeyRepeat,
    KeyChar,
    KeyUp,
    MouseAction,
    LeftButtonDown,
    LeftButtonUp,
    LeftButtonClick,
    LeftButtonDoubleClick,
    MiddleButtonDown,
    MiddleButtonUp,
    MiddleButtonClick,
    MiddleButtonDoubleClick,
    RightButtonDown,
    RightButtonUp,
    RightButtonClick,
    RightButtonDoubleClick,
    WindowResized,
    WindowMoved,
    WindowCreated,
    WindowDestroyed,
    WindowShown,
    WindowHidden,
} AwMsgType;

typedef struct AwMsg;

typedef int32_t (*AwMsgHandler)(AwMsg* msg);

typedef struct {
    int16_t     left;       // inclusive left edge of the rectangle (this is inside the rectangle)
    int16_t     top;        // inclusive top edge of the rectangle (this is inside the rectangle)
    int16_t     right;      // exclusive right edge of the rectangle (this is outside the rectangle)
    int16_t     bottom;     // exclusive bottom edge of the rectangle (this is outside the rectangle)
} AwRect;

typedef struct {
    uint8_t         border : 1;     // whether the window has a border
    uint8_t         title_bar : 1;  // whether the window has a title bar
    uint8_t         icons : 1;      // whether the window has icons in the title bar
    uint8_t         sizeable : 1;   // whether the window can be resized
    uint8_t         active : 1;     // whether the window is active
    uint8_t         enabled : 1;    // whether the window is enabled
    uint8_t         selected : 1;   // whether the window is selected
    uint8_t         visible : 1;    // whether the window is visible
} AwWindowFlags;

typedef struct {
    AwMsgHandler    msg_handler;    // points to the message handler function
    AwWindow*       parent;         // points to the parent window
    AwWindow*       first_child;    // points to the first child window
    AwWindow*       next_sibling;   // points to the next sibling window
    char*           text;           // title of the window or text content
    uint16_t        class_id;       // non-unique class ID for the window
    uint16_t        context_id;     // VDP context ID for the window
    AwRect          window_rect;    // rectangle enclosing the entire window
    AwRect          client_rect;    // rectangle enclosing the client area
    AwWindowFlags   flags;          // flags describing the window
} AwWindow;

typedef struct {
    uint8_t     border : 1;     // paint the border
    uint8_t     title_bar : 1;  // paint the title bar
    uint8_t     title : 1;      // paint the title
    uint8_t     icons: 1;       // paint the icons in the title bar
    uint8_t     background : 1; // paint the client background
    uint8_t     foreground : 1; // paint the client foreground
    uint8_t     enabled : 1;    // whether the window is enabled
    uint8_t     selected : 1;   // whether the window is selected
} AwPaintFlags;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintWindow;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintBorder;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintTitleBar;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintTitle;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintIcons;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintClient;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintBackground;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintForeground;

typedef struct {
    AwWindow*       window;
    uint8_t         key_down : 1;       // whether any key is currently pressed
    uint8_t         key_repeat : 1;     // whether any key is currently being repeated
    uint8_t         left_button_down;   // whether the left mouse button is currently held down
    uint8_t         middle_button_down; // whether the middle mouse button is currently held down
    uint8_t         right_button_down;  // whether the right mouse button is currently held down
    uint8_t         ctrl : 1;           // whether the CTRL key is currently pressed
    uint8_t         shift : 1;          // whether the SHIFT key is currently pressed
    uint8_t         alt : 1;            // whether the ALT key is currently pressed
} AwInputState;

typedef struct {
    uint8_t         down : 1;           // whether the key/button is now down
    uint8_t         up : 1;             // whether the key/button is now up
    uint8_t         repeat : 1;         // whether the key is being repeated
    uint8_t         click : 1;          // whether the button was clicked
    uint8_t         double_click : 1;   // whether the button was double-clicked
    uint8_t         move : 1;           // whether the mouse is being moved
    uint8_t         reserved1 : 1;      // reserved
    uint8_t         reserved2 : 1;      // reserved
} AwInputAction;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
    AwInputAction   action;
    uint16_t        key_code;
    int16_t         key_char;
} AwMsgKeyAction;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
    uint16_t        key_code;
    int16_t         key_char;
} AwMsgKeyDown;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
    uint16_t        key_code;
    int16_t         key_char;
} AwMsgKeyRepeat;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
    uint16_t        key_code;
    int16_t         key_char;
} AwMsgKeyChar;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
    uint16_t        key_code;
    int16_t         key_char;
} AwMsgKeyUp;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgMouseAction;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgLeftButtonDown;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgLeftButtonUp;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgLeftButtonClick;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgLeftButtonDoubleClick;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgMiddleButtonDown;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgMiddleButtonUp;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgMiddleButtonClick;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgMiddleButtonDoubleClick;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgRightButtonDown;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgRightButtonUp;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgRightButtonClick;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwInputState    state;
} AwMsgRightButtonDoubleClick;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
} AwMsgWindowResized;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
} AwMsgWindowMoved;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
} AwMsgWindowCreated;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
} AwMsgWindowDestroyed;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
} AwMsgWindowShown;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
} AwMsgWindowHidden;

typedef union {
    AwMsgPaintBackground    paint_background;
    AwMsgPaintBorder        paint_border;
    AwMsgPaintTitleBar      paint_title_bar;
    AwMsgPaintTitle         paint_title;
    AwMsgPaintIcons         paint_icons;
    AwMsgPaintForeground    paint_foreground;
    AwMsgKeyAction          key_action;
    AwMsgKeyDown            key_down;
    AwMsgKeyRepeat          key_repeat;
    AwMsgKeyChar            key_char;
    AwMsgKeyUp              key_up;
    AwMsgMouseAction        mouse_action;
    AwMsgLeftButtonDown     left_button_down;
    AwMsgLeftButtonUp       left_button_up;
    AwMsgLeftButtonClick    left_button_click;
    AwMsgLeftButtonDoubleClick left_button_double_click;
    AwMsgMiddleButtonDown   middle_button_down;
    AwMsgMiddleButtonUp     middle_button_up;
    AwMsgMiddleButtonClick  middle_button_click;
    AwMsgMiddleButtonDoubleClick middle_button_double_click;
    AwMsgRightButtonDown    right_button_down;
    AwMsgRightButtonUp      right_button_up;
    AwMsgRightButtonClick   right_button_click;
    AwMsgRightButtonDoubleClick right_button_double_click;
    AwMsgWindowResized      window_resized;
    AwMsgWindowMoved        window_moved;
    AwMsgWindowCreated      window_created;
    AwMsgWindowDestroyed    window_destroyed;
    AwMsgWindowShown        window_shown;
    AwMsgWindowHidden       window_hidden;
} AwMsg;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
