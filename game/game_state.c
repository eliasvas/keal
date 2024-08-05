#define STB_IMAGE_IMPLEMENTATION
#include "game_state.h"
#include "engine.h"
#include "tileset4922.inl"

static GameState gs = {0};

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


oglImage game_load_rgba_image_from_disk(const char *path) {
    oglImage img;
    s32 w,h,comp;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(path, &w, &h, &comp, STBI_rgb_alpha);
    if(stbi_failure_reason()) {
        printf("Failed reading image: %s\n", stbi_failure_reason());
    }
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    assert(ogl_image_init(&img, image, w, h, OGL_IMAGE_FORMAT_RGBA8U));
    stbi_image_free(image);
    return img;
}


void game_state_init_images() {
    gs.atlas = game_load_rgba_image_from_disk("/home/ily/Desktop/engine/build/Debug/assets/tileset4922.png");
    u32 white = 0xFFFF;
    ogl_image_init(&gs.white, (u8*)(&white), 1, 1, OGL_IMAGE_FORMAT_R8U);
}

void game_state_init() {
    game_state_init_images();
    nentity_manager_init(&gs.em);
    ntransform_cm_init(&gs.tcm, &gs.em);

    // ECS test
    {
        nEntity parent = nentity_create(&gs.em);
        nEntity child = nentity_create(&gs.em);
        ntransform_cm_add(&gs.tcm, parent, 0);
        ntransform_cm_set_local(&gs.tcm, ntransform_cm_lookup(&gs.tcm, parent), m4d(6.0));
        nTransformComponent *c = ntransform_cm_add(&gs.tcm, child, parent);
        ntransform_cm_set_local(&gs.tcm, ntransform_cm_lookup(&gs.tcm, child), m4d(2.0));
        ntransform_cm_simulate(&gs.tcm);

        ntransform_cm_del(&gs.tcm, parent);
        assert(!NCOMPONENT_INDEX_VALID(ntransform_cm_lookup(&gs.tcm, parent)));
        assert(!NCOMPONENT_INDEX_VALID(ntransform_cm_lookup(&gs.tcm, child)));
    }
    // Init some GUI stuff
    sprintf(wdata.name, "Debug");
    wdata.dim = gv2(400,300);
    wdata.pos = gv2(100,100);
    wdata.active = 0;


}

void game_state_deinit() {
    ntransform_cm_deinit(&gs.tcm, &gs.em);
    nentity_manager_destroy(&gs.em);
}

void game_state_update_and_render() {
    nbatch2d_rend_begin(&gs.batch_rend, &get_ngs()->win);
    vec4 colors[15] = { v4(0.95f, 0.61f, 0.73f, 1.0f), v4(0.55f, 0.81f, 0.95f, 1.0f), v4(0.68f, 0.85f, 0.90f, 1.0f), v4(0.67f, 0.88f, 0.69f, 1.0f), v4(1.00f, 0.78f, 0.49f, 1.0f), v4(0.98f, 0.93f, 0.36f, 1.0f), v4(1.00f, 0.63f, 0.48f, 1.0f), v4(0.55f, 0.81f, 0.25f, 1.0f), v4(0.85f, 0.44f, 0.84f, 1.0f), v4(0.94f, 0.90f, 0.55f, 1.0f), v4(0.80f, 0.52f, 0.25f, 1.0f), v4(0.70f, 0.13f, 0.13f, 1.0f), v4(0.56f, 0.93f, 0.56f, 1.0f), v4(0.93f, 0.51f, 0.93f, 1.0f), v4(0.95f, 0.61f, 0.73f, 1.0f) };
    rand_init();
    for (u32 i = 0; i < 32; i+=1) {
        for (u32 j = 0; j < 32; j+=1) {
            nBatch2DQuad q = {0};
            q.color = colors[gen_random(0,14)];
            q.pos.x = 32 * i;
            q.pos.y = 32 * j;
            q.dim.x = 32;
            q.dim.y = 32;
            q.tc = v4(TILESET_RES_W*TILESET_STEP_X*gen_random(0,32), TILESET_RES_H*TILESET_STEP_Y*gen_random(0,32), TILESET_RES_W*TILESET_STEP_X, -TILESET_RES_H*TILESET_STEP_Y);
            q.angle_rad = 10*sin(get_current_timestamp()/1000.0);
            nbatch2d_rend_add_quad(&gs.batch_rend, q, &gs.atlas);
        }
    }

    // FIXME -- i think we crash if nothing is drawn?
    nbatch2d_rend_end(&gs.batch_rend);

    do_gui_test();
}