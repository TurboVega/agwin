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

// Default message handler for AW_CLASS_MESSAGE_BOX windows

#include "agwinmbox.h"
#include "agwincore.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

int32_t aw_message_box_win_msg_handler(AwWindow* window, AwMsg* msg, bool* halt) {
    (void)window; // presently unused
    (void)halt; // presently unused
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

        case Aw_Do_UnloadApp: {
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

                case Aw_On_RealTimeClockEvent: {
                    break;
                }

                case Aw_On_TimerEvent: {
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
