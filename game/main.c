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
    nDebugNameCM dcm;
    ndebug_name_cm_init(&dcm, &em);
    nEntity e;
    for (u32 i = 0; i < 2000; ++i) {
        e = nentity_create(&em);
        assert(nentity_alive(&em, e));
        nDebugNameComponent *c = ndebug_name_cm_add_entity(&dcm, e);
        sprintf(c->name, "name_%d", i);
        nentity_destroy(&em, e);
        nDebugNameComponent lc = ndebug_name_cm_lookup_entity(&dcm, e);
        assert(!nentity_alive(&em, e));
        assert(strcmp(lc.name, c->name) == 0);
    }
    ndebug_name_cm_del_entity(&dcm, 1024);
    ndebug_name_cm_update(&dcm);
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

