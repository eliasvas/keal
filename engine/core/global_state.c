// FIXME -- this _IMPLEMENTATION macro should be in the (future) asset system
#define STB_IMAGE_IMPLEMENTATION
#include "global_state.h"

static EngineGlobalState global_state;

EngineGlobalState* get_engine_global_state() {
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

void engine_global_state_init() {
    // Track engine start time
    global_state.engine_start_ts = get_current_timestamp();
    // Initialize the frame arena
    global_state.frame_arena = arena_alloc();
    // Initialize default FPS to 0.1 (uncapped)
    global_state.target_fps = 0.1;
    // Initialize frame counter to 0
    global_state.frame_count = 0;
}

void engine_global_state_frame_begin() {
    global_state.frame_start_ts = get_current_timestamp();
    //printf("current ts: %llu\n", global_state.frame_start_ts);
}

void engine_global_state_frame_end() {
    // FIXME: Why busy wait?
    while (((f64)(get_current_timestamp() - global_state.frame_start_ts))/1000.0 < (1.0/global_state.target_fps)){};

    global_state.frame_end_ts = get_current_timestamp();
    global_state.dt = global_state.frame_end_ts - global_state.frame_start_ts;
    arena_clear(global_state.frame_arena);
    global_state.frame_count+=1;
}

void engine_global_state_set_target_fps(f64 target_fps) {
    global_state.target_fps = target_fps;
}

f64 engine_global_state_get_dt() {
    return global_state.dt;
}

