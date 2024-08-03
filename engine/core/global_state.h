#ifndef nglobal_state_H
#define nglobal_state_H

#include "base/base_inc.h"
#include "gfx/ogl.h"
#include "core/window.h"

typedef struct nGlobalState nGlobalState;
struct nGlobalState {
    // TODO -- should we have TWO arenas and swap them out?
    Arena *frame_arena; // this is cleared every frame
    Arena *global_arena; // this is NOT

    u64 engine_start_ts;
    f64 target_fps;

    u64 frame_count;
    u64 frame_start_ts, frame_end_ts;
    f64 dt;

    // rendering stuff
    oglContext ogl_ctx;
    nWindow win;
};

nGlobalState* get_gs();
Arena* get_global_arena();
Arena* get_frame_arena();
u64 get_global_frame_count();

void nglobal_state_init();
void nglobal_state_frame_begin();
void nglobal_state_frame_end();
void nglobal_state_set_target_fps(f64 target_fps);
f64  nglobal_state_get_dt();

#endif