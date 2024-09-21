#include "sprite.h"

nSprite nsprite_make(vec4 start_tc, u32 frame_count, u32 fps, vec4 color) {
    nSprite s = {0};
    s.start_tc = start_tc;
    s.frame_count = frame_count;
    s.fps = fps;
    s.color = color;

    return s;
}

vec4 nsprite_get_current_tc(nSprite *sprite) {
    vec4 tc = sprite->start_tc;
    tc.x += floor(sprite->frame);
    return tc;
}

void nsprite_update(nSprite *sprite, f32 dt) {
    sprite->progress +=  dt / (1.0 / sprite->fps);
    if (sprite->progress > 1.0) {
        sprite->progress -= 1.0;
    }
    sprite->frame =  dt / (1.0 / sprite->fps);
}