#include "window.h"

vec2 nwindow_get_dim(nWindow *win) {
    return v2(win->ww, win->wh);
}

b32 nwindow_init(nWindow *win, const char *name, f32 ww, f32 wh, nWindowOptions opt) {
    win->ww = ww;
    win->wh = wh;
    win->opt = opt;
    sprintf(win->name, name);
    return nwindow_create(win);
}

b32 nwindow_create(nWindow *win) {
    return nwindow_impl_create(win);
}

b32 nwindow_deinit(nWindow *win) {
    return nwindow_impl_destroy(win);
}

void nwindow_swap(nWindow *win) {
    nwindow_impl_swap(win);
}


nWindowEventNode* nwindow_capture_events(nWindow *win) {
    return nwindow_impl_capture_events(win);
}