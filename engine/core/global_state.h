#ifndef ENGINE_GLOBAL_STATE_H_
#define ENGINE_GLOBAL_STATE_H_

#include "base/base_inc.h"

typedef struct EngineGlobalState EngineGlobalState;
struct EngineGlobalState {
    // TODO -- should we have TWO arenas and swap them out?
    Arena *frame_arena; // this is cleaned every frame

    u64 engine_start_ts;
    f64 target_fps;

    u64 frame_start_ts, frame_end_ts;
    f64 dt;
};

void engine_global_state_init();
void engine_global_state_frame_begin();
void engine_global_state_frame_end();
void engine_global_state_set_target_fps(f64 target_fps);
f64  engine_global_state_get_dt();

#endif