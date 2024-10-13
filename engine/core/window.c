#include "window.h"
#include "core/global_state.h"

// TODO -- these externs should probably be function pointers that are set based to some option
extern b32 nwindow_impl_create(nWindow *win);
extern b32 nwindow_impl_destroy(nWindow *win);
extern void nwindow_impl_swap(nWindow *win);
extern void nwindow_impl_update_size(nWindow *win);
extern void nwindow_impl_toggle_fullscreen(nWindow *win);
extern nWindowEventNode* nwindow_impl_capture_events(nWindow *win);

vec2 nwindow_get_dim(nWindow *win) {
    return v2(win->ww, win->wh);
}

b32 nwindow_create(nWindow *win) {
    return nwindow_impl_create(win);
}

b32 nwindow_init(nWindow *win, const char *name, f32 ww, f32 wh, nWindowOptions opt) {
    win->ww = ww;
    win->wh = wh;
    win->opt = opt;
    strncpy(win->name, name, sizeof(win->name) - 1);
    return nwindow_create(win);
}

b32 nwindow_deinit(nWindow *win) {
    return nwindow_impl_destroy(win);
}

void nwindow_swap(nWindow *win) {
    nwindow_impl_swap(win);
}

void nwindow_toggle_fullscreen(nWindow *win) {
    nwindow_impl_toggle_fullscreen(win);
}


extern nWindowEventNode* nwindow_impl_capture_events(nWindow *win);
nWindowEventNode* nwindow_capture_events(nWindow *win) {
    return nwindow_impl_capture_events(win);
}