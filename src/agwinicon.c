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

// Default message handler for AW_CLASS_ICON windows

#include "agwinicon.h"
#include "agwincore.h"
#include <agon/vdp_vdu.h>

#ifdef __CPLUSPLUS
extern "C" {
#endif

#define V(color)    (((uint8_t)(color))|0xC0)  /* makes pixel color visible */

#define K   V(0x00)  // black
#define R   V(0x20)  // dark red
#define G   V(0x08)  // dark green
#define Y   V(0x28)  // dark yellow
#define B   V(0x02)  // dark blue
#define M   V(0x22)  // dark magenta
#define C   V(0x0A)  // dark cyan
#define z   V(0x2A)  // light gray
#define Z   V(0x15)  // dark gray
#define r   V(0x30)  // light red
#define g   V(0x0C)  // light green
#define y   V(0x3C)  // light yellow
#define b   V(0x03)  // light blue
#define m   V(0x33)  // light magenta
#define c   V(0x0F)  // light cyan
#define w   V(0x3F)  // white
#define _   0        // invisible

const uint8_t icon_close[AW_ICON_HEIGHT][AW_ICON_WIDTH] = {
    { _,_,_,_,_,_,_,_,_,_ },
    { _,K,K,_,_,_,_,K,K,_ },
    { _,K,w,K,_,_,K,w,K,_ },
    { _,_,K,w,K,K,w,K,_,_ },
    { _,_,_,K,w,w,K,_,_,_ },
    { _,_,_,K,w,w,K,_,_,_ },
    { _,_,K,w,K,K,w,K,_,_ },
    { _,K,w,K,_,_,K,w,K,_ },
    { _,K,K,_,_,_,_,K,K,_ },
    { _,_,_,_,_,_,_,_,_,_ },
};

const uint8_t icon_menu[AW_ICON_HEIGHT][AW_ICON_WIDTH] = {
    { _,_,_,_,_,_,_,_,_,_ },
    { _,_,_,_,_,_,_,_,_,_ },
    { _,K,K,K,K,K,K,K,K,_ },
    { _,K,w,w,w,w,w,w,K,_ },
    { _,K,K,K,K,K,K,K,K,_ },
    { _,K,w,w,w,w,w,w,K,_ },
    { _,K,K,K,K,K,K,K,K,_ },
    { _,K,w,w,w,w,w,w,K,_ },
    { _,K,K,K,K,K,K,K,K,_ },
    { _,_,_,_,_,_,_,_,_,_ },
};

const uint8_t icon_minimize[AW_ICON_HEIGHT][AW_ICON_WIDTH] = {
    { _,_,_,_,_,_,_,_,_,_ },
    { _,_,_,_,_,_,_,_,_,_ },
    { _,_,_,_,_,_,_,_,_,_ },
    { _,_,_,_,_,_,_,_,_,_ },
    { _,K,K,K,K,K,K,K,K,_ },
    { _,K,w,w,w,w,w,w,K,_ },
    { _,K,K,K,K,K,K,K,K,_ },
    { _,_,_,_,_,_,_,_,_,_ },
    { _,_,_,_,_,_,_,_,_,_ },
    { _,_,_,_,_,_,_,_,_,_ },
};

const uint8_t icon_maximize[AW_ICON_HEIGHT][AW_ICON_WIDTH] = {
    { _,_,_,_,_,_,_,_,_,_ },
    { _,K,K,K,K,K,K,K,K,_ },
    { _,K,w,w,w,w,w,w,K,_ },
    { _,K,w,K,K,K,K,w,K,_ },
    { _,K,w,K,K,K,K,w,K,_ },
    { _,K,w,K,K,K,K,w,K,_ },
    { _,K,w,K,K,K,K,w,K,_ },
    { _,K,w,w,w,w,w,w,K,_ },
    { _,K,K,K,K,K,K,K,K,_ },
    { _,_,_,_,_,_,_,_,_,_ },
};

const uint8_t icon_restore[AW_ICON_HEIGHT][AW_ICON_WIDTH] = {
    { _,_,_,_,_,_,_,_,_,_ },
    { K,K,K,K,K,K,K,_,_,_ },
    { K,w,w,w,w,w,K,_,_,_ },
    { K,w,K,K,K,w,K,_,_,_ },
    { K,w,K,w,w,w,w,w,w,_ },
    { K,w,K,w,K,K,K,K,w,_ },
    { K,w,w,w,K,K,K,K,w,_ },
    { _,_,_,w,K,K,K,K,w,_ },
    { _,_,_,w,K,K,K,K,w,_ },
    { _,_,_,w,w,w,w,w,w,_ },
};

void register_icon(uint16_t icon_id, const uint8_t* pixels) {
    vdp_adv_clear_buffer(icon_id);
    vdp_adv_write_block_data(icon_id, AW_ICON_WIDTH * AW_ICON_HEIGHT, (char*) pixels);
    vdp_adv_select_bitmap(icon_id);
    vdp_adv_bitmap_from_buffer(AW_ICON_WIDTH, AW_ICON_HEIGHT, 1);
}

void aw_register_icons() {
    register_icon(AW_ICON_CLOSE, (const uint8_t*) icon_close);
    register_icon(AW_ICON_MENU, (const uint8_t*) icon_menu);
    register_icon(AW_ICON_MINIMIZE, (const uint8_t*) icon_minimize);
    register_icon(AW_ICON_MAXIMIZE, (const uint8_t*) icon_maximize);
    register_icon(AW_ICON_RESTORE, (const uint8_t*) icon_restore);
}

void aw_draw_icon(uint16_t icon_id, int16_t xpos, int16_t ypos) {
    vdp_adv_select_bitmap(icon_id);
    vdp_draw_bitmap(xpos, ypos);
}

int32_t aw_icon_win_msg_handler(AwWindow* window, AwMsg* msg, bool* halt) {
    switch (msg->do_common.msg_type) {
        case Aw_Do_Common: {
            break;
        }

        case Aw_Do_ResizeWindow: {
            break;
        }

        case Aw_Do_MoveWindow: {
            break;
        }

        case Aw_Do_CloseWindow: {
            break;
        }

        case Aw_Do_DestroyWindow: {
            break;
        }

        case Aw_Do_ShowWindow: {
            break;
        }

        case Aw_Do_EnableWindow: {
            break;
        }

        case Aw_Do_ActivateWindow: {
            break;
        }

        case Aw_Do_InvalidateWindow: {
            break;
        }

        case Aw_Do_InvalidateWindowRect: {
            break;
        }

        case Aw_Do_InvalidateClient: {
            break;
        }

        case Aw_Do_InvalidateClientRect: {
            break;
        }

        case Aw_Do_PaintWindow: {
            break;
        }

        case Aw_Do_Terminate: {
            break;
        }

        case Aw_Do_Exit: {
            break;
        }

        default: {
            switch (msg->on_common.msg_type) {

                case Aw_On_Common: {
                    break;
                }

                case Aw_On_KeyEvent: {
                    break;
                }

                case Aw_On_LeftButtonDown: {
                    break;
                }

                case Aw_On_LeftButtonUp: {
                    break;
                }

                case Aw_On_LeftButtonClick: {
                    break;
                }

                case Aw_On_LeftButtonLongClick: {
                    break;
                }

                case Aw_On_LeftButtonDoubleClick: {
                    break;
                }

                case Aw_On_MiddleButtonDown: {
                    break;
                }

                case Aw_On_MiddleButtonUp: {
                    break;
                }

                case Aw_On_MiddleButtonClick: {
                    break;
                }

                case Aw_On_MiddleButtonLongClick: {
                    break;
                }

                case Aw_On_MiddleButtonDoubleClick: {
                    break;
                }

                case Aw_On_RightButtonDown: {
                    break;
                }

                case Aw_On_RightButtonUp: {
                    break;
                }

                case Aw_On_RightButtonClick: {
                    break;
                }

                case Aw_On_RightButtonLongClick: {
                    break;
                }

                case Aw_On_RightButtonDoubleClick: {
                    break;
                }

                case Aw_On_MouseMoved: {
                    break;
                }

                case Aw_On_MouseDragged: {
                    break;
                }

                case Aw_On_MouseDropped: {
                    break;
                }

                case Aw_On_WindowResized: {
                    break;
                }

                case Aw_On_WindowMoved: {
                    break;
                }

                case Aw_On_WindowCreated: {
                    break;
                }

                case Aw_On_WindowDestroyed: {
                    break;
                }

                case Aw_On_WindowShown: {
                    break;
                }

                case Aw_On_WindowEnabled: {
                    break;
                }

                case Aw_On_WindowActivated: {

                }
            
                case Aw_On_Terminate: {
                    break;
                }

                default: {
                    break;
                }
            }
        }
    }

    return 0;
}

#ifdef __CPLUSPLUS
} // extern "C"
#endif
