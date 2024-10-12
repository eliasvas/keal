#include <stdio.h>
#include "engine.h"
#include "game_state.h"

//#define PHYSICS_TEST 1
void physics_test_init();
void physics_test_update_and_render();

//#define GRAPHICS_TEST 1
void gfx_test_init();
void gfx_test_update_and_render();

GameState game_state;

void mainLoop(void) {
    nglobal_state_frame_begin();

#if PHYSICS_TEST
    physics_test_update_and_render();
#elif GRAPHICS_TEST
    gfx_test_update_and_render();
#else
    game_state_update_and_render(&game_state);
#endif

    nglobal_state_frame_end();
}

int main(int argc, char **argv) {
    nglobal_state_init();

#if PHYSICS_TEST
    physics_test_init();
#elif GRAPHICS_TEST
    gfx_test_init();
#else
    game_state_init(&game_state);
#endif

    #ifdef __EMSCRIPTEN__
        EM_ASM({ Module.wasmTable = wasmTable; });
        emscripten_set_main_loop(mainLoop, 0, 1);
    #else
        while (1) { mainLoop(); }
    #endif
}

