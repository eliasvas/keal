#ifndef NSPRITE_H 
#define NSPRITE_H

#include "base/base_inc.h"
#include "core/core_inc.h"

// TODO -- should start_tc be base_tc + sprite_dim?
// TODO -- add 'playing'/'autoplay' fields for less hassle

typedef struct nSprite nSprite;
struct nSprite {
    vec4 start_tc;
    u32 frame_count;
    u32 fps;

    vec4 color;
    // These data change each frame to DO animation
    f32 frame;
    f32 progress;
    // flip stuff
    b32 hflip;
    b32 vflip;
};

nSprite nsprite_make(vec4 start_tc, u32 frame_count, u32 fps, vec4 color);
vec4 nsprite_get_current_tc(nSprite *sprite);
void nsprite_update(nSprite *sprite, f32 dt);
#endif