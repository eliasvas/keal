#include "input.h"
static nInputManager global_input_manager;

void ninput_manager_consume_events_from_window(nWindow *win) {
    nWindowEventNode* event_queue = nwindow_capture_events(win);
    u64 frame_idx = get_global_frame_count();
    for (nWindowEventNode *n = event_queue; n != 0; n = n->next) {
        switch(n->kind) {
            case(N_WINDOW_EVENT_KIND_KEYBOARD_EVENT):
                if (n->ke.key > 128)break;
                global_input_manager.state[n->ke.key].last_state = n->ke.state;
                global_input_manager.state[n->ke.key].frame = frame_idx;
                break;
            case(N_WINDOW_EVENT_KIND_MOUSE_EVENT):
                global_input_manager.state[n->me.key].last_state = n->me.state;
                global_input_manager.state[n->ke.key].frame = frame_idx;
                break;
            case(N_WINDOW_EVENT_KIND_MOUSE_MOTION_EVENT):
                global_input_manager.mouse_pos = v2(n->mme.x,n->mme.y);
                break;
            default:
                assert(0);
                break;
        }
    }
}

b32 ninput_is_key_in_state(u32 key, nInputKeyState state) {
    nInputKeyboardKeyInfo ki = global_input_manager.state[key];
    nInputKeyState actual_state;
    if (get_global_frame_count() == ki.frame) {
        return ((state & 1) == ki.last_state);
    }else {
        return (((state == N_INPUT_KEY_STATE_DOWN) && ki.last_state) || ((state == N_INPUT_KEY_STATE_UP) && !ki.last_state));
    }
}

b32 ninput_key_up(u32 key) {
    return ninput_is_key_in_state(key, N_INPUT_KEY_STATE_UP);
}
b32 ninput_key_down(u32 key) {
    return ninput_is_key_in_state(key, N_INPUT_KEY_STATE_DOWN);
}
b32 ninput_key_pressed(u32 key) {
    return ninput_is_key_in_state(key, N_INPUT_KEY_STATE_PRESSED);
}
b32 ninput_key_released(u32 key) {
    return ninput_is_key_in_state(key, N_INPUT_KEY_STATE_RELEASED);
}





