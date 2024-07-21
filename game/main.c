#include <stdio.h>
#include "engine.h"

int main() {
    // arena_test();
    // arena_scratch_test();
    // sll_stack_test();
    // sll_queue_test();
    // dll_test();

    engine_global_state_init();
    engine_global_state_set_target_fps(12.0);
    platform_init();
    vec4 color = (vec4){1,1,1,1};
    InstanceData data[2];
    b32 flicker_val = 0;
    while(1) {
        engine_global_state_frame_begin();

        //----
        data[0] = (InstanceData){100,100,200,200,0,0,0,0,color.x, color.y,color.z,color.w,0,0,flicker_val & 1};
        data[1] = (InstanceData){200,200,300,300,0,0,0,0,color.x, color.y,color.z,color.w,0,0,!(flicker_val & 1)};
        flicker_val += 1;
        platform_update();
        platform_render(data, 2);
        //----

        engine_global_state_frame_end();
    }
}