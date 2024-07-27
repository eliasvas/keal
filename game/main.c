#include <stdio.h>
#include "engine.h"
#include "game_state.h"

int main(int argc, char **argv) {
    nWindow win = {0};
    engine_global_state_init();
    nwindow_init(&win, "gudGame", 800, 600, N_WINDOW_OPT_RESIZABLE | N_WINDOW_OPT_BORDERLESS);
    engine_global_state_set_target_fps(60.0);
    oglContext ogl_ctx;
    ogl_ctx_init(&ogl_ctx);

    game_state_init(&win);

    nEntityManager em;
    nentity_manager_init(&em);
    nEntity e;
    for (u32 i = 0 ; i < 1026; ++i) {
        e = nentity_create(&em);
        assert(nentity_alive(&em, e));
        nentity_destroy(&em, e);
    }
    nentity_manager_destroy(&em);

    while(1) {
        ninput_manager_consume_events_from_window(&win);
        
        ogl_clear_all_state(&ogl_ctx);
        engine_global_state_frame_begin();
        ogl_image_clear(NULL);

        game_state_update_and_render();

        nwindow_swap(&win);
        engine_global_state_frame_end();
    }
    nwindow_deinit(&win);
}

