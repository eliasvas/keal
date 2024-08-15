#ifndef WINDOW_H
#define WINDOW_H
#include "base/base_inc.h"

////////////////////////////////
// Window
////////////////////////////////

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

////////////////////////////////
// Window Events
////////////////////////////////

typedef enum nWindowEventKind nWindowEventKind;
enum nWindowEventKind {
    N_WINDOW_EVENT_KIND_NONE,
    N_WINDOW_EVENT_KIND_KEYBOARD_EVENT,
    N_WINDOW_EVENT_KIND_MOUSE_EVENT,
    N_WINDOW_EVENT_KIND_MOUSE_MOTION_EVENT,
    N_WINDOW_EVENT_KIND_SCROLLWHEEL_EVENT,
};

typedef struct nWindowKeyboardEvent nWindowKeyboardEvent;
struct nWindowKeyboardEvent {
    u32 key;
    b32 state;
};

typedef struct nWindowMouseEvent nWindowMouseEvent;
struct nWindowMouseEvent {
    u32 key; // 0 = LMB, 1 = MMB, 2 = RMB
    b32 state;
};

typedef struct nWindowMouseMotionEvent nWindowMouseMotionEvent;
struct nWindowMouseMotionEvent {
    f32 x;
    f32 y;
};

typedef struct nWindowScrollWheelEvent nWindowScrollWheelEvent;
struct nWindowScrollWheelEvent {
    f32 y; // scroll amount
};


typedef struct nWindowEventNode nWindowEventNode;
struct nWindowEventNode {
    nWindowEventNode *next;
    union {
        nWindowKeyboardEvent ke;
        nWindowMouseEvent me;
        nWindowMouseMotionEvent mme;
        nWindowScrollWheelEvent swe;
    };
    nWindowEventKind kind;
};


b32 nwindow_init(nWindow *win, const char *name, f32 ww, f32 wh, nWindowOptions opt);
b32 nwindow_deinit(nWindow *win);
vec2 nwindow_get_dim(nWindow *win);
void nwindow_swap(nWindow *win);
nWindowEventNode* nwindow_capture_events(nWindow *win);

#endif