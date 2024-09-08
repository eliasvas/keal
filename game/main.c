#include <stdio.h>
#include "engine.h"
#include "game_state.h"

#define PHYSICS_TEST
void physics_test_init();
void physics_test_update_and_render();

void mainLoop(void) {
    nglobal_state_frame_begin();

#ifdef PHYSICS_TEST
    physics_test_update_and_render();
#else
    game_state_update_and_render();
#endif

    nglobal_state_frame_end();
}

int main(int argc, char **argv) {
    nglobal_state_init();

#ifdef PHYSICS_TEST
    physics_test_init();
#else
    game_state_init();
#endif

    #ifdef __EMSCRIPTEN__
        EM_ASM({ Module.wasmTable = wasmTable; });
        emscripten_set_main_loop(mainLoop, 0, 1);
    #else
        while (1) { mainLoop(); }
    #endif
}

