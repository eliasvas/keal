#ifndef INPUT_H
#define INPUT_H
// TODO -- This needs a LOT of work
// we don't even support non-ASCII keys dammit!
#include "base/base_inc.h"
#include "core/core_inc.h"

#define N_INPUT_MAX_MOUSE_KEYS 3
#define N_INPUT_LMB 0
#define N_INPUT_MMB 1
#define N_INPUT_RMB 2

typedef enum nInputKeyState nInputKeyState;
enum nInputKeyState {
    N_INPUT_KEY_STATE_RELEASED = 0,
    N_INPUT_KEY_STATE_PRESSED = 1,
    N_INPUT_KEY_STATE_UP = 2,
    N_INPUT_KEY_STATE_DOWN = 3,
};

typedef struct nInputKeyboardKeyInfo nInputKeyboardKeyInfo;
struct nInputKeyboardKeyInfo {
    nInputKeyState last_state;
    u64 frame; // in which frame we got the last state change
};

typedef nInputKeyboardKeyInfo nInputMouseKeyInfo;

typedef struct nInputManager nInputManager;
struct nInputManager {
    nInputKeyboardKeyInfo state[128];
    vec2 mouse_pos;
};

void ninput_manager_consume_events_from_window(nWindow *win);

b32 ninput_key_up(u32 key);
b32 ninput_key_down(u32 key);
b32 ninput_key_pressed(u32 key);
b32 ninput_key_released(u32 key);

#endif