#include "awfcn.h"

int get_rect_width(const AwRect* rect) {
    return rect->right - rect->left;
}

int get_rect_height(const AwRect* rect) {
    return rect->bottom - rect->top;
}

AwWindow* get_desktop_window() {

}

AwWindow* get_top_level_window(const AwWindow* window) {

}

AwWindow* create_window(AwWindow* parent, uint16_t class_id, AwWindowFlags flags,
                        uint16_t width, uint16_t height, const char* text) {
    
}

void destroy_window(AwWindow* window) {

}
