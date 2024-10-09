#ifndef NGLOBAL_STATE_H
#define NGLOBAL_STATE_H

#include "base/base_inc.h"
#include "gfx/ogl.h"
#include "core/window.h"
#include "core/input.h"
#include "audio/audio.h"

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

    // audio stuff
    nAudioContext actx;

    // input stuff
    nInputManager im;

    // window stuff (only one window supported currently!)
    nWindow win;
};

nGlobalState* get_ngs();
Arena* get_global_arena();
Arena* get_frame_arena();
u64 get_global_frame_count();

void nglobal_state_init();
void nglobal_state_frame_begin();
void nglobal_state_frame_end();
void nglobal_state_set_target_fps(f64 target_fps);
f64  nglobal_state_get_dt();
f64  nglobal_state_get_dt_sec();


// Let's one global instead of singletons
nWindow *get_nwin();
nAudioContext *get_nactx();
nInputManager *get_nim();

#endif