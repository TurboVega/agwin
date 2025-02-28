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

// Default message handler for AW_CLASS_ICON windows

#include "agwinmsg.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

#define AW_ICON_WIDTH  10
#define AW_ICON_HEIGHT 10

#define AW_ICON_CLOSE      ((uint16_t)63900)
#define AW_ICON_MENU       ((uint16_t)63901)
#define AW_ICON_MINIMIZE   ((uint16_t)63902)
#define AW_ICON_MAXIMIZE   ((uint16_t)63903)
#define AW_ICON_RESTORE    ((uint16_t)63904)

void aw_register_icons();

void aw_draw_icon(uint16_t icon_id, int16_t xpos, int16_t ypos);

int32_t aw_icon_win_msg_handler(AwWindow* window, AwMsg* msg, bool* halt);

#ifdef __CPLUSPLUS
} // extern "C"
#endif
