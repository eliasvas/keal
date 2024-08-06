#include <stdio.h>
#include "engine.h"
#include "game_state.h"

void mainLoop(void) {
    ninput_manager_consume_events_from_window(&get_ngs()->win);
    gui_impl_update();
    
    ogl_clear_all_state(&get_ngs()->ogl_ctx);
    nglobal_state_frame_begin();
    ogl_image_clear(NULL);

    game_state_update_and_render();
    gui_impl_render();

    nglobal_state_frame_end();
}

int main(int argc, char **argv) {
    nglobal_state_init();
    game_state_init();
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(mainLoop, 0, 1);
    #else
        while (1) { mainLoop(); }
    #endif
}

