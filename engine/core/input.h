#ifndef INPUT_H
#define INPUT_H
// TODO -- API for scroll-wheel!
// TODO -- ditch global input_manager, maybe have one in every window?? IDK
#include "base/base_inc.h"
#include "core/core_inc.h"


// TODO -- maybe these should be called NMKEY_ for N<MOUSE>KEY_
typedef enum nKeyMouseKeycode nKeyMouseKeycode;
enum nKeyMouseKeycode {
    NKEY_LMB = 0,
    NKEY_MMB = 1,
    NKEY_RMB = 2,
    NKEY_COUNT,
};

// This is equivalent to SDL_Scancode / USB keyboard spec's scancodes
typedef enum nKeyScanCode nKeyScanCode;
enum nKeyScanCode {
    NKEY_SCANCODE_UNKNOWN = 0,
    NKEY_SCANCODE_A = 4,
    NKEY_SCANCODE_B = 5,
    NKEY_SCANCODE_C = 6,
    NKEY_SCANCODE_D = 7,
    NKEY_SCANCODE_E = 8,
    NKEY_SCANCODE_F = 9,
    NKEY_SCANCODE_G = 10,
    NKEY_SCANCODE_H = 11,
    NKEY_SCANCODE_I = 12,
    NKEY_SCANCODE_J = 13,
    NKEY_SCANCODE_K = 14,
    NKEY_SCANCODE_L = 15,
    NKEY_SCANCODE_M = 16,
    NKEY_SCANCODE_N = 17,
    NKEY_SCANCODE_O = 18,
    NKEY_SCANCODE_P = 19,
    NKEY_SCANCODE_Q = 20,
    NKEY_SCANCODE_R = 21,
    NKEY_SCANCODE_S = 22,
    NKEY_SCANCODE_T = 23,
    NKEY_SCANCODE_U = 24,
    NKEY_SCANCODE_V = 25,
    NKEY_SCANCODE_W = 26,
    NKEY_SCANCODE_X = 27,
    NKEY_SCANCODE_Y = 28,
    NKEY_SCANCODE_Z = 29,

    NKEY_SCANCODE_1 = 30,
    NKEY_SCANCODE_2 = 31,
    NKEY_SCANCODE_3 = 32,
    NKEY_SCANCODE_4 = 33,
    NKEY_SCANCODE_5 = 34,
    NKEY_SCANCODE_6 = 35,
    NKEY_SCANCODE_7 = 36,
    NKEY_SCANCODE_8 = 37,
    NKEY_SCANCODE_9 = 38,
    NKEY_SCANCODE_0 = 39,

    NKEY_SCANCODE_RETURN = 40,
    NKEY_SCANCODE_ESCAPE = 41,
    NKEY_SCANCODE_BACKSPACE = 42,
    NKEY_SCANCODE_TAB = 43,
    NKEY_SCANCODE_SPACE = 44,

    NKEY_SCANCODE_MINUS = 45,
    NKEY_SCANCODE_EQUALS = 46,
    NKEY_SCANCODE_LEFTBRACKET = 47,
    NKEY_SCANCODE_RIGHTBRACKET = 48,
    NKEY_SCANCODE_BACKSLASH = 49,
    NKEY_SCANCODE_NONUSHASH = 50,
    NKEY_SCANCODE_SEMICOLON = 51,
    NKEY_SCANCODE_APOSTROPHE = 52,
    NKEY_SCANCODE_GRAVE = 53,
    NKEY_SCANCODE_COMMA = 54,
    NKEY_SCANCODE_PERIOD = 55,
    NKEY_SCANCODE_SLASH = 56,
    NKEY_SCANCODE_CAPSLOCK = 57,

    NKEY_SCANCODE_F1 = 58,
    NKEY_SCANCODE_F2 = 59,
    NKEY_SCANCODE_F3 = 60,
    NKEY_SCANCODE_F4 = 61,
    NKEY_SCANCODE_F5 = 62,
    NKEY_SCANCODE_F6 = 63,
    NKEY_SCANCODE_F7 = 64,
    NKEY_SCANCODE_F8 = 65,
    NKEY_SCANCODE_F9 = 66,
    NKEY_SCANCODE_F10 = 67,
    NKEY_SCANCODE_F11 = 68,
    NKEY_SCANCODE_F12 = 69,

    NKEY_SCANCODE_PRINTSCREEN = 70,
    NKEY_SCANCODE_SCROLLLOCK = 71,
    NKEY_SCANCODE_PAUSE = 72,
    NKEY_SCANCODE_INSERT = 73,
    NKEY_SCANCODE_HOME = 74,
    NKEY_SCANCODE_PAGEUP = 75,
    NKEY_SCANCODE_DELETE = 76,
    NKEY_SCANCODE_END = 77,
    NKEY_SCANCODE_PAGEDOWN = 78,
    NKEY_SCANCODE_RIGHT = 79,
    NKEY_SCANCODE_LEFT = 80,
    NKEY_SCANCODE_DOWN = 81,
    NKEY_SCANCODE_UP = 82,
    NKEY_SCANCODE_COUNT,
};

typedef enum nInputKeyState nInputKeyState;
enum nInputKeyState {
    N_INPUT_KEY_STATE_RELEASED = 0,
    N_INPUT_KEY_STATE_PRESSED = 1,
    N_INPUT_KEY_STATE_UP = 2,
    N_INPUT_KEY_STATE_DOWN = 3,
};

typedef struct nInputKeyboardKeyInfo nInputKeyboardKeyInfo;
struct nInputKeyboardKeyInfo {
    nInputKeyState last_state; //the state the key was at frame 'frame'
    u64 frame; // in which frame we got the last state change
};

typedef nInputKeyboardKeyInfo nInputMouseKeyInfo;

typedef struct nInputManager nInputManager;
struct nInputManager {
    nInputKeyboardKeyInfo keyboard_state[128];
    nInputKeyboardKeyInfo mouse_state[3];

    vec2 mouse_pos;
    vec2 prev_mouse_pos;
};

void ninput_manager_consume_events_from_window(nWindow *win);

b32 ninput_key_up(nKeyScanCode key);
b32 ninput_key_down(nKeyScanCode key);
b32 ninput_key_pressed(nKeyScanCode key);
b32 ninput_key_released(nKeyScanCode key);

b32 ninput_mkey_up(nKeyMouseKeycode key);
b32 ninput_mkey_down(nKeyMouseKeycode key);
b32 ninput_mkey_pressed(nKeyMouseKeycode key);
b32 ninput_mkey_released(nKeyMouseKeycode key);

vec2 ninput_get_mouse_delta(void);
vec2 ninput_get_mouse_pos(void);

#endif