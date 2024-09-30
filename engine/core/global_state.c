#include "global_state.h"
#include "misc/gui_impl.h"

static nGlobalState global_state = {0};

nGlobalState* get_ngs() {
    return &global_state;
}

Arena* get_frame_arena() {
    return (global_state.frame_arena);
}

Arena* get_global_arena() {
    return (global_state.global_arena);
}

u64 get_global_frame_count() {
    return (global_state.frame_count);
}


void nglobal_state_init() {
    // Track engine start time
    global_state.engine_start_ts = get_current_timestamp();
    // Initialize the frame arena
    global_state.frame_arena = arena_alloc();
    // Initialize the global arena
    global_state.global_arena = arena_alloc();
    // Initialize default FPS to 0.1 (uncapped)
    global_state.target_fps = 0.1;
    // Initialize frame counter to 0
    global_state.frame_count = 0;

    // Init random generator
    rand_init();

    nwindow_init(&global_state.win, "roguelite_prototype", 1024, 768, N_WINDOW_OPT_RESIZABLE | N_WINDOW_OPT_BORDERLESS);
    nglobal_state_set_target_fps(60.0);
    ogl_ctx_init(&global_state.ogl_ctx);
    naudio_context_init(&global_state.actx);
    ninput_manager_init(&global_state.im);
    gui_impl_init();
}

void nglobal_state_frame_begin() {
    global_state.frame_start_ts = get_current_timestamp();
    // Consume input events from window and update GUI state
    ninput_manager_consume_events_from_window(get_nim(), get_nwin());
    gui_impl_update();

    // Then clear graphics state/screen
    ogl_clear_all_state(get_nogl_ctx());
    ogl_image_clear(NULL);
}

void nglobal_state_frame_end() {
    // Render GUI
    gui_impl_render();
    // Swap the backbuffers (display to screen)
    nwindow_swap(&global_state.win);
    // FIXME: Why busy wait?
    while (((f64)(get_current_timestamp() - global_state.frame_start_ts))/1000.0 < (1.0/global_state.target_fps)){};

    global_state.frame_end_ts = get_current_timestamp();
    global_state.dt = global_state.frame_end_ts - global_state.frame_start_ts;
    arena_clear(global_state.frame_arena);
    global_state.frame_count+=1;
}

void nglobal_state_set_target_fps(f64 target_fps) {
    global_state.target_fps = target_fps;
}

f64 nglobal_state_get_dt() {
    return global_state.dt;
}
f64 nglobal_state_get_dt_sec() {
    return nglobal_state_get_dt()/1000.0;
}

nWindow *get_nwin() {
    return &global_state.win;
}
oglContext *get_nogl_ctx() {
    return &global_state.ogl_ctx;
}
nAudioContext *get_nactx() {
    return &global_state.actx;
}
nInputManager *get_nim() {
    return &global_state.im;
}