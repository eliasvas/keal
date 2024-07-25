#ifndef WINDOW_H
#define WINDOW_H
#include "base/base_inc.h"

typedef enum nWindowOptions nWindowOptions;
enum nWindowOptions {
    N_WINDOW_OPT_NONE       = (0 << 0),
    N_WINDOW_OPT_RESIZABLE  = (1 << 0),
    N_WINDOW_OPT_BORDERLESS = (1 << 1),
};

typedef struct nWindow nWindow;
struct nWindow {
    f32 ww,wh;
    char name[64];
    nWindowOptions opt;

    // This is a pointer for the implementation to use (it can hold e.g SDL_Window)
    void *impl_state;
};

typedef enum nWindowEventKind nWindowEventKind;
enum nWindowEventKind {
    N_WINDOW_EVENT_KIND_NONE,
    N_WINDOW_EVENT_KIND_KEYBOARD_ACTION,
    N_WINDOW_EVENT_KIND_MOUSE_ACTION,
};

typedef struct nWindowEventNode nWindowEventNode;
struct nWindowEventNode {
    nWindowEventKind kind;
    s32 param0;
    s32 param1;
    nWindowEventNode *next;
};



b32 nwindow_init(nWindow *win, const char *name, f32 ww, f32 wh, nWindowOptions opt);
b32 nwindow_deinit(nWindow *win);
vec2 nwindow_get_dim(nWindow *win);
void nwindow_swap(nWindow *win);
nWindowEventNode* nwindow_capture_events(nWindow *win);

#endif