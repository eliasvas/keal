#include <stdio.h>
#include "engine.h"
#include "game_state.h"

int main(int argc, char **argv) {
    nglobal_state_init();
    game_state_init();

    while(1) {
        ninput_manager_consume_events_from_window(&get_gs()->win);
        gui_impl_update();
        
        ogl_clear_all_state(&get_gs()->ogl_ctx);
        nglobal_state_frame_begin();
        ogl_image_clear(NULL);

        game_state_update_and_render();
        gui_impl_render();

        nglobal_state_frame_end();
    }
}

