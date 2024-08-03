#include <stdio.h>
#include "engine.h"
#include "game_state.h"

guiSimpleWindowData wdata= {0};
guiSliderData spin_data;

void do_gui_test() {
    if (ninput_mkey_pressed(NKEY_MMB)){
        wdata.active = (wdata.active) ? 0 : 1;
    }
    gui_build_begin();
    guiVec4 colors[15] = { gv4(0.95f, 0.61f, 0.73f, 1.0f), gv4(0.55f, 0.81f, 0.95f, 1.0f), gv4(0.68f, 0.85f, 0.90f, 1.0f), gv4(0.67f, 0.88f, 0.69f, 1.0f), gv4(1.00f, 0.78f, 0.49f, 1.0f), gv4(0.98f, 0.93f, 0.36f, 1.0f), gv4(1.00f, 0.63f, 0.48f, 1.0f), gv4(0.55f, 0.81f, 0.25f, 1.0f), gv4(0.85f, 0.44f, 0.84f, 1.0f), gv4(0.94f, 0.90f, 0.55f, 1.0f), gv4(0.80f, 0.52f, 0.25f, 1.0f), gv4(0.70f, 0.13f, 0.13f, 1.0f), gv4(0.56f, 0.93f, 0.56f, 1.0f), gv4(0.93f, 0.51f, 0.93f, 1.0f), gv4(0.95f, 0.61f, 0.73f, 1.0f), };
    if (wdata.active){
        gui_swindow_begin(&wdata);
        gui_set_next_bg_color(gv4(0.6,0.2,0.4,1.0));
        gui_set_next_pref_width((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0,1.0});
        gui_set_next_pref_height((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0/5.0,0.5});
        gui_spinner("spinner123", AXIS2_X, gv2(0,10), &spin_data);


        for (u32 i = 0; i < 4; ++i) {
            char panel_name[128];
            sprintf(panel_name,"panel_abc%d", i);
            gui_set_next_child_layout_axis(AXIS2_X);
            gui_set_next_pref_width((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0,1.0});
            gui_set_next_pref_height((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0/5.0,1.0});
            guiSignal s = gui_panel(panel_name);
            gui_push_parent(s.box);
            for (u32 j = i; j < 5; ++j) {
                char button_name[128];
                sprintf(button_name, "b%d%d", i, j);
                gui_set_next_bg_color(colors[i*(j-1)]);
                gui_set_next_pref_width((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0,0.0});
                gui_set_next_pref_height((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0,1.0});
                gui_button(button_name);
            }
            gui_pop_parent();
        }
        gui_swindow_end(&wdata);
    }
    gui_build_end();
}

int main(int argc, char **argv) {
    nglobal_state_init();
    game_state_init(&get_gs()->win);

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

    // GUI window example
    sprintf(wdata.name, "Debug");
    wdata.dim = gv2(400,300);
    wdata.pos = gv2(100,100);
    wdata.active = 0;

    while(1) {
        ninput_manager_consume_events_from_window(&(get_gs()->win));
        gui_impl_update();
        
        ogl_clear_all_state(&get_gs()->ogl_ctx);
        nglobal_state_frame_begin();
        ogl_image_clear(NULL);

        game_state_update_and_render();
        do_gui_test();
        gui_impl_render();

        nglobal_state_frame_end();
    }
}

