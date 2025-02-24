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
    Terminate,
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
    int16_t     x;          // X coordinate
    int16_t     y;          // Y coordinate
} AwPoint;

typedef struct {
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
    uint16_t        reserved1 : 1;  // reserved
    uint16_t        reserved2 : 1;  // reserved
    uint16_t        reserved3 : 1;  // reserved
    uint16_t        reserved4 : 1;  // reserved
    uint16_t        reserved5 : 1;  // reserved
    uint16_t        reserved6 : 1;  // reserved
} AwWindowFlags;

typedef struct {
    const char*     name;           // name of the app
    AwMsgHandler    msg_handler;    // points to the message handler function
    uint32_t*       load_address;   // location that the app was loaded
    uint32_t        memory_size;    // amount of memory allocated to app
} AwApplication;

typedef struct {
    AwApplication*  app;            // points to the app that owns the window
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
    uint16_t        border : 1;     // paint the border
    uint16_t        title_bar : 1;  // paint the title bar
    uint16_t        title : 1;      // paint the title
    uint16_t        icons: 1;       // paint the icons in the title bar
    uint16_t        background : 1; // paint the client background
    uint16_t        foreground : 1; // paint the client foreground
    uint16_t        enabled : 1;    // whether the window is enabled
    uint16_t        selected : 1;   // whether the window is selected
    uint16_t        reserved1 : 1;  // reserved
    uint16_t        reserved2 : 1;  // reserved
    uint16_t        reserved3 : 1;  // reserved
    uint16_t        reserved4 : 1;  // reserved
    uint16_t        reserved5 : 1;  // reserved
    uint16_t        reserved6 : 1;  // reserved
    uint16_t        reserved7 : 1;  // reserved
    uint16_t        reserved8 : 1;  // reserved
} AwPaintFlags;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwRect          paint_rect;
    AwPaintFlags    flags;
} AwMsgPaint;

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
    AwPoint         mouse_screen_pt;    // Mouse location relative to entire screen
    AwPoint         mouse_window_pt;    // Mouse location relative to window beneath it
    AwPoint         mouse_client_pt;    // Mouse location relative to client area beneath it
} AwMsgInputAction;

typedef struct {
    AwWindow*       window;
    AwMsgType       msg_type;
} AwMsgSimple;

typedef AwMsgPaint AwMsgPaintWindow;
typedef AwMsgPaint AwMsgPaintBorder;
typedef AwMsgPaint AwMsgPaintTitleBar;
typedef AwMsgPaint AwMsgPaintTitle;
typedef AwMsgPaint AwMsgPaintIcons;
typedef AwMsgPaint AwMsgPaintClient;
typedef AwMsgPaint AwMsgPaintBackground;
typedef AwMsgPaint AwMsgPaintForeground;

typedef AwMsgInputAction AwMsgKeyAction;
typedef AwMsgInputAction AwMsgKeyDown;
typedef AwMsgInputAction AwMsgKeyRepeat;
typedef AwMsgInputAction AwMsgKeyChar;
typedef AwMsgInputAction AwMsgKeyUp;
typedef AwMsgInputAction AwMsgMouseAction;
typedef AwMsgInputAction AwMsgLeftButtonDown;
typedef AwMsgInputAction AwMsgLeftButtonUp;
typedef AwMsgInputAction AwMsgLeftButtonClick;
typedef AwMsgInputAction AwMsgLeftButtonDoubleClick;
typedef AwMsgInputAction AwMsgMiddleButtonDown;
typedef AwMsgInputAction AwMsgMiddleButtonUp;
typedef AwMsgInputAction AwMsgMiddleButtonClick;
typedef AwMsgInputAction AwMsgMiddleButtonDoubleClick;
typedef AwMsgInputAction AwMsgRightButtonDown;
typedef AwMsgInputAction AwMsgRightButtonUp;
typedef AwMsgInputAction AwMsgRightButtonClick;
typedef AwMsgInputAction AwMsgRightButtonDoubleClick;

typedef AwMsgSimple AwMsgWindowResized;
typedef AwMsgSimple AwMsgWindowMoved;
typedef AwMsgSimple AwMsgWindowCreated;
typedef AwMsgSimple AwMsgWindowDestroyed;
typedef AwMsgSimple AwMsgWindowShown;
typedef AwMsgSimple AwMsgWindowHidden;
typedef AwMsgSimple AwMsgTerminate;

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
    AwMsgTerminate          terminate;
} AwMsg;

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
