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

    nEntityManager em = {0};
    nentity_manager_init(&em);
    nTransformCM tcm = {0};
    ntransform_cm_init(&tcm, &em);
    {
        nEntity parent = nentity_create(&em);
        nEntity child = nentity_create(&em);
        ntransform_cm_add(&tcm, parent, 0);
        ntransform_cm_set_local(&tcm, ntransform_cm_lookup(&tcm, parent), m4d(6.0));
        nTransformComponent *c = ntransform_cm_add(&tcm, child, parent);
        ntransform_cm_set_local(&tcm, ntransform_cm_lookup(&tcm, child), m4d(2.0));
        ntransform_cm_simulate(&tcm);

        ntransform_cm_del(&tcm, parent);
        assert(!NCOMPONENT_INDEX_VALID(ntransform_cm_lookup(&tcm, parent)));
        assert(!NCOMPONENT_INDEX_VALID(ntransform_cm_lookup(&tcm, child)));
    }
    ntransform_cm_deinit(&tcm, &em);
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

