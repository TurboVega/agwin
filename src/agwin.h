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

typedef struct {
    int16_t     left;       // inclusive left edge of the rectangle (this is inside the rectangle)
    int16_t     top;        // inclusive top edge of the rectangle (this is inside the rectangle)
    int16_t     right;      // exclusive right edge of the rectangle (this is outside the rectangle)
    int16_t     bottom;     // exclusive bottom edge of the rectangle (this is outside the rectangle)
} AwRect;

typedef struct {
    uint8_t     border : 1;     // paint the border
    uint8_t     title_bar : 1;  // paint the title bar
    uint8_t     title : 1;      // paint the title
    uint8_t     icons: 1;       // paint the icons in the title bar
    uint8_t     background : 1; // paint the client background
    uint8_t     foreground : 1; // paint the client foreground
    uint8_t     reserved1 : 1;  // reserved
    uint8_t     reserved2 : 1;  // reserved
} AwPaintFlags;

typedef struct {
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintWindow;

typedef struct {
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintBorder;

typedef struct {
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintTitleBar;

typedef struct {
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintTitle;

typedef struct {
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintIcons;

typedef struct {
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintClient;

typedef struct {
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintBackground;

typedef struct {
    AwMsgType       msg_type;
    AwRect          win_rect;
    AwRect          client_rect;
    AwPaintFlags    flags;
} AwMsgPaintForeground;

typedef struct {
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
    AwMsgType       msg_type;
} AwMsgKeyAction;

typedef struct {
    AwMsgType       msg_type;
} AwMsgKeyDown;

typedef struct {
    AwMsgType       msg_type;
} AwMsgKeyRepeat;

typedef struct {
    AwMsgType       msg_type;
} AwMsgKeyChar;

typedef struct {
    AwMsgType       msg_type;
} AwMsgKeyUp;

typedef struct {
    AwMsgType       msg_type;
} AwMsgMouseAction;

typedef struct {
    AwMsgType       msg_type;
} AwMsgLeftButtonDown;

typedef struct {
    AwMsgType       msg_type;
} AwMsgLeftButtonUp;

typedef struct {
    AwMsgType       msg_type;
} AwMsgLeftButtonClick;

typedef struct {
    AwMsgType       msg_type;
} AwMsgLeftButtonDoubleClick;

typedef struct {
    AwMsgType       msg_type;
} AwMsgMiddleButtonDown;

typedef struct {
    AwMsgType       msg_type;
} AwMsgMiddleButtonUp;

typedef struct {
    AwMsgType       msg_type;
} AwMsgMiddleButtonClick;

typedef struct {
    AwMsgType       msg_type;
} AwMsgMiddleButtonDoubleClick;

typedef struct {
    AwMsgType       msg_type;
} AwMsgRightButtonDown;

typedef struct {
    AwMsgType       msg_type;
} AwMsgRightButtonUp;

typedef struct {
    AwMsgType       msg_type;
} AwMsgRightButtonClick;

typedef struct {
    AwMsgType       msg_type;
} AwMsgRightButtonDoubleClick;

typedef struct {
    AwMsgType       msg_type;
} AwMsgWindowResized;

typedef struct {
    AwMsgType       msg_type;
} AwMsgWindowMoved;

typedef struct {
    AwMsgType       msg_type;
} AwMsgWindowCreated;

typedef struct {
    AwMsgType       msg_type;
} AwMsgWindowDestroyed;

typedef struct {
    AwMsgType       msg_type;
} AwMsgWindowShown;

typedef struct {
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
    AwMsgMiddleButtonDown   middle_button_down;
    AwMsgMiddleButtonUp     middle_button_up;
    AwMsgRightButtonDown    right_button_down;
    AwMsgRightButtonUp      right_button_up;
    AwMsgWindowResized      window_resized;
    AwMsgWindowMoved        window_moved;
    AwMsgWindowCreated      window_created;
    AwMsgWindowDestroyed    window_destroyed;
    AwMsgWindowShown        window_shown;
    AwMsgWindowHidden       window_hidden;
} AwMsg;

int get_rect_width(const AwRect* rect) { return rect->right - rect->left; }
int get_rect_height(const AwRect* rect) { return rect->bottom - rect->top; }

#pragma pack(pop)

#ifdef __CPLUSPLUS
} // extern "C"
#endif
