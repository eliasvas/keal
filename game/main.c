#include <stdio.h>
#include "engine.h"

int main() {
    arena_test();
    arena_scratch_test();
    // platform_init();
    // vec4 color = (vec4){1,1,1,1};
    // InstanceData data[2];
    // b32 flicker_val = 0;
    // while(1) {
    //     data[0] = (InstanceData){100,100,200,200,0,0,0,0,color.x, color.y,color.z,color.w,0,0,flicker_val & 1};
    //     data[1] = (InstanceData){200,200,300,300,0,0,0,0,color.x, color.y,color.z,color.w,0,0,!(flicker_val & 1)};
    //     flicker_val += 1;
    //     platform_update();
    //     platform_render(data, 2);
    // }
}