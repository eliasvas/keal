#ifndef ENGINE_GLOBAL_STATE_H
#define ENGINE_GLOBAL_STATE_H

#include "base/base_inc.h"

typedef struct EngineGlobalState EngineGlobalState;
struct EngineGlobalState {
    // TODO -- should we have TWO arenas and swap them out?
    Arena *frame_arena; // this is cleaned every frame
    Arena *global_arena; // this is NOT

    u64 engine_start_ts;
    f64 target_fps;

    u64 frame_start_ts, frame_end_ts;
    f64 dt;
};

EngineGlobalState* get_engine_global_state();
Arena* get_global_arena();
Arena* get_frame_arena();

void engine_global_state_init();
void engine_global_state_frame_begin();
void engine_global_state_frame_end();
void engine_global_state_set_target_fps(f64 target_fps);
f64  engine_global_state_get_dt();

#endif