#include "input.h"

extern u64 get_global_frame_count(void);

void ninput_manager_init(nInputManager *im) {
    M_ZERO_STRUCT(im);
}

void ninput_manager_consume_events_from_window(nInputManager *im, nWindow *win) {
    im->prev_mouse_pos = im->mouse_pos;
    im->prev_scroll_amount = im->scroll_amount;
    nWindowEventNode* event_queue = nwindow_capture_events(win);
    u64 frame_idx = get_global_frame_count();
    for (nWindowEventNode *n = event_queue; n != 0; n = n->next) {
        switch(n->kind) {
            case(N_WINDOW_EVENT_KIND_KEYBOARD_EVENT):
                if (n->ke.key > NKEY_SCANCODE_COUNT)break;
                im->keyboard_state[n->ke.key].last_state = n->ke.state;
                im->keyboard_state[n->ke.key].frame = frame_idx;
                break;
            case(N_WINDOW_EVENT_KIND_MOUSE_EVENT):
                im->mouse_state[n->me.key].last_state = n->me.state;
                im->mouse_state[n->ke.key].frame = frame_idx;
                break;
            case(N_WINDOW_EVENT_KIND_MOUSE_MOTION_EVENT):
                im->mouse_pos = v2(n->mme.x,n->mme.y);
                break;
            case(N_WINDOW_EVENT_KIND_SCROLLWHEEL_EVENT):
                im->scroll_amount += n->swe.y;
                break;
            default:
                assert(0);
                break;
        }
    }
}

b32 ninput_is_key_in_state(nInputManager *im, nKeyScanCode key, nInputKeyState state) {
    nInputKeyboardKeyInfo ki = im->keyboard_state[key];
    nInputKeyState actual_state;
    if (get_global_frame_count() == ki.frame) {
        return ((state & 1) == ki.last_state);
    }else {
        return (((state == N_INPUT_KEY_STATE_DOWN) && ki.last_state) || ((state == N_INPUT_KEY_STATE_UP) && !ki.last_state));
    }
}

b32 ninput_key_up(nInputManager *im, nKeyScanCode key) {
    return ninput_is_key_in_state(im, key, N_INPUT_KEY_STATE_UP);
}
b32 ninput_key_down(nInputManager *im, nKeyScanCode key) {
    return ninput_is_key_in_state(im, key, N_INPUT_KEY_STATE_DOWN);
}
b32 ninput_key_pressed(nInputManager *im, nKeyScanCode key) {
    return ninput_is_key_in_state(im, key, N_INPUT_KEY_STATE_PRESSED);
}
b32 ninput_key_released(nInputManager *im, nKeyScanCode key) {
    return ninput_is_key_in_state(im, key, N_INPUT_KEY_STATE_RELEASED);
}

b32 ninput_is_mkey_in_state(nInputManager *im, nKeyMouseKeycode key, nInputKeyState state) {
    nInputMouseKeyInfo ki = im->mouse_state[key];
    nInputKeyState actual_state;
    if (get_global_frame_count() == ki.frame) {
        return ((state & 1) == ki.last_state);
    }else {
        return (((state == N_INPUT_KEY_STATE_DOWN) && ki.last_state) || ((state == N_INPUT_KEY_STATE_UP) && !ki.last_state));
    }
}

b32 ninput_mkey_up(nInputManager *im, nKeyMouseKeycode key) {
    return ninput_is_mkey_in_state(im, key, N_INPUT_KEY_STATE_UP);
}
b32 ninput_mkey_down(nInputManager *im, nKeyMouseKeycode key) {
    return ninput_is_mkey_in_state(im, key, N_INPUT_KEY_STATE_DOWN);
}
b32 ninput_mkey_pressed(nInputManager *im, nKeyMouseKeycode key) {
    return ninput_is_mkey_in_state(im, key, N_INPUT_KEY_STATE_PRESSED);
}
b32 ninput_mkey_released(nInputManager *im, nKeyMouseKeycode key) {
    return ninput_is_mkey_in_state(im, key, N_INPUT_KEY_STATE_RELEASED);
}

vec2 ninput_get_mouse_pos(nInputManager *im) {
    return im->mouse_pos;
}

vec2 ninput_get_mouse_delta(nInputManager *im) {
    return vec2_sub(im->mouse_pos, im->prev_mouse_pos);
}


nScrollAmount ninput_get_scroll_amount_delta(nInputManager *im) {
    return (im->scroll_amount - im->prev_scroll_amount);
}

nScrollAmount ninput_get_scroll_amount(nInputManager *im) {
    return im->scroll_amount;
}
