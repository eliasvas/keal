#define STB_IMAGE_IMPLEMENTATION
#include "game_state.h"
#include "engine.h"
#include "tileset4922.inl"

/*
    GAME TODO:

    To reach a point where we can start playing around with GUI/editor stuff I need the game loop
    to be better. We need a bit more things.

    - There should be droppable items we can pick up and put in inventory (spell scrolls/potions)
    - We should be able to have a 'scroll list' of items (interface will be Q+ E- SPACE to use said item)
    - Tiles should be pickable so you can 'cast' spells
    - There should be stairs somewhere in the dungeon to signify end of the level

    Maybe once these + 3D +++++ all other stuff are done we will have
    a teleglitch style game (i want real time combat REAL bad)
*/

static GameState gs = {0};

GameState *get_ggs() {
    return &gs;
}

// guiSimpleWindowData wdata = {0};
// guiSliderData spin_data = {0};
// guiSliderData slider_data = {0};
// guiSliderData slider_data2 = {0};

// void do_gui_test() {
//     if (ninput_mkey_pressed(NKEY_MMB)){
//         wdata.active = (wdata.active) ? 0 : 1;
//     }
//     gui_build_begin();
//     guiVec4 colors[15] = { gv4(0.95f, 0.61f, 0.73f, 1.0f), gv4(0.55f, 0.81f, 0.95f, 1.0f), gv4(0.68f, 0.85f, 0.90f, 1.0f), gv4(0.67f, 0.88f, 0.69f, 1.0f), gv4(1.00f, 0.78f, 0.49f, 1.0f), gv4(0.98f, 0.93f, 0.36f, 1.0f), gv4(1.00f, 0.63f, 0.48f, 1.0f), gv4(0.55f, 0.81f, 0.25f, 1.0f), gv4(0.85f, 0.44f, 0.84f, 1.0f), gv4(0.94f, 0.90f, 0.55f, 1.0f), gv4(0.80f, 0.52f, 0.25f, 1.0f), gv4(0.70f, 0.13f, 0.13f, 1.0f), gv4(0.56f, 0.93f, 0.56f, 1.0f), gv4(0.93f, 0.51f, 0.93f, 1.0f), gv4(0.95f, 0.61f, 0.73f, 1.0f), };
//     if (wdata.active){
//         gui_swindow_begin(&wdata, AXIS2_Y);
        
//         gui_set_next_pref_width((guiSize){GUI_SIZEKIND_CHILDREN_SUM,1.0,0.2});
//         gui_set_next_pref_height((guiSize){GUI_SIZEKIND_CHILDREN_SUM,1.0,0.2});
//         gui_set_next_child_layout_axis(AXIS2_X);
//         guiSignal sp = gui_panel("spinner_panel");
//         gui_push_parent(sp.box);
//         {
//             gui_set_next_pref_width((guiSize){GUI_SIZEKIND_TEXT_CONTENT,5.0,1.0});
//             gui_set_next_pref_height((guiSize){GUI_SIZEKIND_TEXT_CONTENT,5.0,1.0});
//             guiSignal s = gui_label("min_room_size (px)");
//             gui_set_next_bg_color(gv4(0.6,0.2,0.4,1.0));
//             gui_set_next_pref_width((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0,1.0});
//             gui_set_next_pref_height((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0/5.0,0.5});
//             gui_spinner("spinner123", AXIS2_X, gv2(8,16), &spin_data);
//         }
//         gui_pop_parent();

//         gui_set_next_bg_color(gv4(0.6,0.2,0.4,1.0));
//         gui_set_next_pref_width((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0,1.0});
//         gui_set_next_pref_height((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0/5.0,0.5});
//         gui_slider("min_room_factor", AXIS2_X, gv2(4,10), &slider_data);
       
//         gui_set_next_bg_color(gv4(0.6,0.2,0.4,1.0));
//         gui_set_next_pref_width((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0,1.0});
//         gui_set_next_pref_height((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1.0/5.0,0.5});
//         gui_slider("max_room_factor", AXIS2_X, gv2(6,10), &slider_data2);
       
//         gui_set_next_bg_color(gv4(1,0.4,0.4,1.0));
//         gui_set_next_pref_width((guiSize){GUI_SIZEKIND_TEXT_CONTENT,5.0,1.0});
//         gui_set_next_pref_height((guiSize){GUI_SIZEKIND_TEXT_CONTENT,5.0,1.0});
//         guiSignal genb = gui_button("generate map");
//         if (genb.flags & GUI_SIGNAL_FLAG_LMB_PRESSED) {
//             nactor_cm_clear(&(get_ggs()->acm));
//             nmap_create_ex(&gs.map, 64,64, spin_data.value, slider_data.value / 10.0,slider_data2.value / 10.0);
//         }
//         gui_swindow_end(&wdata);
//     }
//     gui_build_end();
// }



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
    gs.atlas = game_load_rgba_image_from_disk("assets/tileset4922.png");
    u32 white = 0xFFFF;
    ogl_image_init(&gs.white, (u8*)(&white), 1, 1, OGL_IMAGE_FORMAT_R8U);
}

nEntity player;
void game_state_init() {
    game_state_status_set(GAME_STATUS_STARTUP);
    gs.zoom_amount = 1;
    gs.animation_speed = 10;
    game_state_init_images();
    nentity_manager_init(&gs.em);
    ntransform_cm_init(&gs.tcm, &gs.em);
    nactor_cm_init(&gs.acm, &gs.em);
    game_state_generate_new_level();
    game_state_status_set(GAME_STATUS_START_MENU);
}

void game_state_deinit() {
    ntransform_cm_deinit(&gs.tcm, &gs.em);
    nentity_manager_destroy(&gs.em);
}

void game_state_update_and_render() {
    do_game_gui();
    if (game_state_status_match(GAME_STATUS_START_MENU)) {return;}
    GameStatus status = nactor_cm_check_movement_event(&(gs.acm)) ? GAME_STATUS_NEW_TURN : GAME_STATUS_IDLE;
    game_state_status_set(status);

    ivec2 player_pos = iv2(0,0);
    nActorComponent *player_cmp = nactor_cm_get(&(gs.acm), gs.map.player);
    if (player_cmp) {
        nmap_compute_fov(&(gs.map), player_cmp->posx, player_cmp->posy, 3);
        player_pos = iv2(player_cmp->posx, player_cmp->posy);
    }

    nactor_cm_simulate(&(gs.acm), &(gs.map), game_state_status_match(GAME_STATUS_NEW_TURN));

    // TODO -- this 0.1 should become scroll speed or something
    nScrollAmount scroll_y = ninput_get_scroll_amount_delta();
    if (scroll_y) {
        gs.zoom_amount += scroll_y * 0.1;
    }

    ivec2 dist_to_mp = iv2(-(player_pos.x *TILESET_DEFAULT_SIZE*gs.zoom_amount- get_ngs()->win.ww/2+TILESET_DEFAULT_SIZE*gs.zoom_amount/2), -(player_pos.y *TILESET_DEFAULT_SIZE*gs.zoom_amount- get_ngs()->win.wh/2+TILESET_DEFAULT_SIZE*gs.zoom_amount/2));
    mat4 view = mat4_mult(mat4_translate(v3(dist_to_mp.x,dist_to_mp.y,0)),mat4_scale(v3(gs.zoom_amount, gs.zoom_amount, 1)));
    nbatch2d_rend_set_view_mat(&gs.batch_rend, view);

    nbatch2d_rend_begin(&gs.batch_rend, &get_ngs()->win);
 
    nmap_render(&(gs.map), &(gs.batch_rend), &(gs.atlas));

    nactor_cm_render(&(gs.acm), &(gs.batch_rend), &(gs.atlas));

    // FIXME -- i think we crash if nothing is drawn? WHY?!
    nbatch2d_rend_end(&gs.batch_rend);

    //do_gui_test();
}

void game_state_status_set(GameStatus status) {
    gs.status = status;
}

b32  game_state_status_match(GameStatus status) {
    return (gs.status == status);
}

void game_state_generate_new_level() {
    nactor_cm_clear(&(get_ggs()->acm));
    nmap_create_ex(&gs.map, 64,64, 8,0.4,0.6);
}