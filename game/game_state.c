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
    gs.dcam.zoom = 1;
    gs.animation_speed = 10;
    game_state_init_images();
    nentity_manager_init(&gs.em);
    ntransform_cm_init(&gs.tcm, &gs.em);
    nactor_cm_init(&gs.acm, &gs.em);
    game_state_generate_new_level();
    game_state_status_set(GAME_STATUS_START_MENU);
    ndungeon_cam_set(&gs.dcam, v2(0,0), v2(3,3));
}

void game_state_deinit() {
    ntransform_cm_deinit(&gs.tcm, &gs.em);
    nentity_manager_destroy(&gs.em);
}

// TODO -- Make these fade in/out based not only on current but previous sprites!
void game_state_render_silly_stuff() {
    rand_init();
    guiVec4 colors[15] = { gv4(0.95f, 0.61f, 0.73f, 1.0f), gv4(0.55f, 0.81f, 0.95f, 1.0f), gv4(0.68f, 0.85f, 0.90f, 1.0f), gv4(0.67f, 0.88f, 0.69f, 1.0f), gv4(1.00f, 0.78f, 0.49f, 1.0f), gv4(0.98f, 0.93f, 0.36f, 1.0f), gv4(1.00f, 0.63f, 0.48f, 1.0f), gv4(0.55f, 0.81f, 0.25f, 1.0f), gv4(0.85f, 0.44f, 0.84f, 1.0f), gv4(0.94f, 0.90f, 0.55f, 1.0f), gv4(0.80f, 0.52f, 0.25f, 1.0f), gv4(0.70f, 0.13f, 0.13f, 1.0f), gv4(0.56f, 0.93f, 0.56f, 1.0f), gv4(0.93f, 0.51f, 0.93f, 1.0f), gv4(0.95f, 0.61f, 0.73f, 1.0f), };
    for (u32 i = 0; i < (get_nwin()->ww /32); i+=1) {
        for (u32 j = 0; j < (get_nwin()->wh / 32); j+=1) {
            nBatch2DQuad q = {0};
            guiVec4 c =colors[gen_random(0,14)];
            q.color = v4(c.x,c.y,c.z,c.w);
            q.pos.x = 32 * i;
            q.pos.y = 32 * j;
            q.dim.x = 32;
            q.dim.y = 32;
            q.tc = v4(TILESET_RES_W*TILESET_STEP_X*gen_random(0,32), TILESET_RES_H*TILESET_STEP_Y*gen_random(0,32), TILESET_RES_W*TILESET_STEP_X, -TILESET_RES_H*TILESET_STEP_Y);
            q.angle_rad = sin(get_current_timestamp_sec()*2)/3;
            vec2 mp = ninput_get_mouse_pos(get_nim());
            if (fabsf(mp.x - q.pos.x - q.dim.x/2) < 10)q.color = v4(1,1,1,1);
            if (fabsf(mp.y - q.pos.y - q.dim.y/2) < 10)q.color = v4(1,1,1,1);
            nbatch2d_rend_add_quad(&gs.batch_rend, q, &gs.atlas);
        }
    }
}
void game_state_render_selection_box() {
    nBatch2DQuad q = {0};
    vec2 mp = ninput_get_mouse_pos(get_nim());
    mp = ndungeon_screen_to_world(&gs.dcam, mp);
    q.color = v4(1,1,1,1);
    q.pos.x = TILESET_DEFAULT_SIZE * (floor(mp.x));
    q.pos.y = TILESET_DEFAULT_SIZE * (floor(mp.y));
    q.dim.x = TILESET_DEFAULT_SIZE;
    q.dim.y = TILESET_DEFAULT_SIZE;
    q.tc = TILESET_SELECTION_BOX_TILE;
    q.angle_rad = 0;
    nbatch2d_rend_add_quad(&gs.batch_rend, q, &gs.atlas);
}

void game_state_render_dir_arrow(vec2 player_pos) {
    nBatch2DQuad q = {0};
    vec2 mp = ninput_get_mouse_pos(get_nim());
    mp = ndungeon_screen_to_world(&gs.dcam, mp);
    mp = vec2_sub(mp, player_pos);
    mp = vec2_norm(mp);
    f32 dist = 1.0;
    f32 x_off = dist * mp.x;
    f32 y_off = dist * mp.y;
    q.color = v4(1,1,1,1);
    q.pos.x = TILESET_DEFAULT_SIZE * (player_pos.x + x_off);
    q.pos.y = TILESET_DEFAULT_SIZE * (player_pos.y + y_off);
    q.dim.x = TILESET_DEFAULT_SIZE;
    q.dim.y = TILESET_DEFAULT_SIZE;
    q.tc = TILESET_RARROW_TILE;
    q.angle_rad = atan2(y_off, x_off);
    nbatch2d_rend_add_quad(&gs.batch_rend, q, &gs.atlas);
}

void game_state_update_and_render() {
    ntest_colliders();
    do_game_gui();
    if (ninput_key_pressed(get_nim(), NKEY_SCANCODE_ESCAPE)) {game_state_status_set(GAME_STATUS_START_MENU);}
    if (ninput_key_pressed(get_nim(), NKEY_SCANCODE_EQUALS)) {nactor_cm_gc(&gs.acm);}
    // if (game_state_status_match(GAME_STATUS_START_MENU)) {
    //     mat4 viewm = m4d(1);
    //     nbatch2d_rend_set_view_mat(&gs.batch_rend, viewm);
    //     nbatch2d_rend_begin(&gs.batch_rend, get_nwin);
    //     game_state_render_silly_stuff();
    //     nbatch2d_rend_end(&gs.batch_rend);
    //     return;
    // }

    if (ninput_key_pressed(get_nim(), NKEY_SCANCODE_SPACE)) {
        game_state_status_set(game_state_status_match(GAME_STATUS_PAUSED) ? GAME_STATUS_RUNNING : GAME_STATUS_PAUSED);
    }

    // GameStatus status = GAME_STATUS_RUNNING;//nactor_cm_check_movement_event(&(gs.acm)) ? GAME_STATUS_RUNNING : GAME_STATUS_PAUSED;
    // game_state_status_set(status);

    ivec2 player_pos = iv2(0,0);
    nActorComponent *player_cmp = nactor_cm_get(&(gs.acm), gs.map.player);
    if (player_cmp) {
        nmap_compute_fov(&(gs.map), player_cmp->posx, player_cmp->posy, 3);
        player_pos = iv2(player_cmp->posx, player_cmp->posy);
    }

    // If game is running, we can simulate!
    if (game_state_status_match(GAME_STATUS_RUNNING)) {
        nactor_cm_simulate(&(gs.acm), &(gs.map), game_state_status_match(GAME_STATUS_RUNNING));
    }
    // If game paused, do editor stuff (TODO -- we should have some form of tween-animation for this transition)
    if (game_state_status_match(GAME_STATUS_PAUSED)) {
        vec2 mp = ninput_get_mouse_pos(get_nim());
        mp = ndungeon_screen_to_world(&gs.dcam, mp);
        ivec2 tile = iv2(floor(mp.x), floor(mp.y));
        nTile *t = nmap_tile_ref(&gs.map, tile.x, tile.y);
        if (t) {
            if (gui_input_mb_down(GUI_LMB)) {
                nmap_dig_region(&gs.map, tile.x, tile.y, tile.x+1, tile.y+1, NTILE_KIND_WALL);
            }
            if (gui_input_mb_down(GUI_RMB)) {
                nmap_dig_region(&gs.map, tile.x, tile.y, tile.x+1, tile.y+1, NTILE_KIND_GROUND);
            }
        }
    }

    if (game_state_status_match(GAME_STATUS_RUNNING) || game_state_status_match(GAME_STATUS_PAUSED)) {
        nScrollAmount scroll_y = ninput_get_scroll_amount_delta(get_nim());
        if (scroll_y) { gs.dcam.zoom += scroll_y * 0.1; }
        ndungeon_cam_update(&gs.dcam, v2(player_pos.x, player_pos.y));
        mat4 view = ndungeon_cam_get_view_mat(&gs.dcam);
        nbatch2d_rend_set_view_mat(&gs.batch_rend, view);
        nbatch2d_rend_begin(&gs.batch_rend, get_nwin());
        nmap_render(&(gs.map), &(gs.batch_rend), &(gs.atlas));
        game_state_render_dir_arrow(v2(player_pos.x,player_pos.y));
        if (game_state_status_match(GAME_STATUS_PAUSED)) {
            game_state_render_selection_box();
        }
        nactor_cm_render(&(gs.acm), &(gs.batch_rend), &(gs.atlas));
        nbatch2d_rend_end(&gs.batch_rend);
    }
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