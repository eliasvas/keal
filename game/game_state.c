#define STB_IMAGE_IMPLEMENTATION
#include "game_state.h"
#include "engine.h"
#include "tileset4922.inl"

static GameState gs = {0};

GameState *get_ggs() {
    return &gs;
}

void update_and_render_sprites(nEntityMgr *em) {
    nbatch2d_rend_begin(&gs.batch_rend, get_nwin());
    mat4 view = ndungeon_cam_get_view_mat(&gs.dcam);
    nbatch2d_rend_set_view_mat(&gs.batch_rend, view);
    for (u32 i = 0; i < em->comp_array_len; i+=1) {
        nEntityID entity = i;

        // simulate movement input stuff only for player
        if (NENTITY_MANAGER_HAS_COMPONENT(em, entity, nPhysicsBody)) {
            nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(em, entity, nPhysicsBody);
            f32 dt = nglobal_state_get_dt()/1000.0;
            if (*(NENTITY_MANAGER_GET_COMPONENT(em, entity, nEntityTag)) == NENTITY_TAG_PLAYER) {
                if (ninput_key_down(get_nim(),NKEY_SCANCODE_RIGHT)) {
                    b->velocity.x+=300*dt; // speed * dt
                }
                if (ninput_key_down(get_nim(),NKEY_SCANCODE_LEFT)) {
                    b->velocity.x-=300*dt; // speed * dt
                }
                if (ninput_key_down(get_nim(),NKEY_SCANCODE_UP)) {
                    b->velocity.y-=300*dt; // speed * dt
                }
                if (ninput_key_down(get_nim(),NKEY_SCANCODE_DOWN)) {
                    b->velocity.y+=300*dt; // speed * dt
                }

            }
        }

        // render the stuff
        if (NENTITY_MANAGER_HAS_COMPONENT(em, entity, nSprite) && NENTITY_MANAGER_HAS_COMPONENT(em, entity, nPhysicsBody)) {
            nSprite *s = NENTITY_MANAGER_GET_COMPONENT(em, entity, nSprite);
            nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(em, entity, nPhysicsBody);
            nsprite_update(s, nglobal_state_get_dt() / 1000.0);
            vec4 tc = nsprite_get_current_tc(s);

            nBatch2DQuad q = {0};
            q.color = s->color;
            q.tc = tc;
            q.pos.x = b->position.x - b->dim.x/2;
            q.pos.y = b->position.y - b->dim.y/2;
            q.dim.x = b->dim.x/2;
            q.dim.y = b->dim.y/2;
            q.angle_rad = 0;
            nbatch2d_rend_add_quad(&gs.batch_rend, q, &gs.atlas);
        }
    }
    nbatch2d_rend_end(&gs.batch_rend);
}

oglImage game_load_rgba_image_from_disk(const char *path) {
    oglImage img;
    s32 w,h,comp;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(path, &w, &h, &comp, STBI_rgb_alpha);
    if(stbi_failure_reason()) {
        NLOG_ERR("Failed reading image: %s\n", stbi_failure_reason());
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

extern void nphysics_world_update_func(nEntityMgr *em);

void game_state_init() {
    game_state_status_set(GAME_STATUS_STARTUP);
    ndungeon_cam_set(&gs.dcam, v2(0,0), v2(0.3,0.3), 1);
    game_state_init_images();

    NENTITY_MANAGER_INIT(get_em());
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nEntityTag);
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nPhysicsBody);
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nSprite);
    NENTITY_MANAGER_ADD_SYSTEM(get_em(), nphysics_world_update_func, 1);
    NENTITY_MANAGER_ADD_SYSTEM(get_em(), update_and_render_sprites, 2);

    game_state_status_set(GAME_STATUS_START_MENU);

    // init player entity
    gs.player = nem_make(get_em());
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), gs.player, nPhysicsBody);
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), gs.player, nSprite);
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), gs.player, nEntityTag); // Maybe tag should be instantiated in nem_make(em)
    *NENTITY_MANAGER_GET_COMPONENT(get_em(), gs.player, nSprite) = nsprite_make(TILESET_ANIM_PLAYER_TILE, 5, 2, v4(1,0,0,1));
    nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs.player, nPhysicsBody);
    *b = nphysics_body_aabb(v2(100,100), 200*gen_rand01());
    b->position = v2(100,100);
    b->gravity_scale = 0;
    nEntityTag *player_tag = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs.player, nEntityTag);
    *player_tag = NENTITY_TAG_PLAYER;
}

void game_state_deinit() {
    // ntransform_cm_deinit(&gs.tcm, &gs.em);
    // nentity_manager_destroy(&gs.em);
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
    // nBatch2DQuad q = {0};
    // vec2 mp = ninput_get_mouse_pos(get_nim());
    // mp = ndungeon_screen_to_world(&gs.dcam, mp);
    // q.color = v4(1,1,1,1);
    // q.pos.x = TILESET_DEFAULT_SIZE * (floor(mp.x));
    // q.pos.y = TILESET_DEFAULT_SIZE * (floor(mp.y));
    // q.dim.x = TILESET_DEFAULT_SIZE;
    // q.dim.y = TILESET_DEFAULT_SIZE;
    // q.tc = TILESET_SELECTION_BOX_TILE;
    // q.angle_rad = 0;
    // nbatch2d_rend_add_quad(&gs.batch_rend, q, &gs.atlas);
}

void game_state_render_dir_arrow(vec2 player_pos) {
    // nBatch2DQuad q = {0};
    // vec2 mp = ninput_get_mouse_pos(get_nim());
    // mp = ndungeon_screen_to_world(&gs.dcam, mp);
    // mp = vec2_sub(mp, player_pos);
    // mp = vec2_norm(mp);
    // f32 dist = 1.0;
    // f32 x_off = dist * mp.x;
    // f32 y_off = dist * mp.y;
    // q.color = v4(1,1,1,1);
    // q.pos.x = TILESET_DEFAULT_SIZE * (player_pos.x + x_off);
    // q.pos.y = TILESET_DEFAULT_SIZE * (player_pos.y + y_off);
    // q.dim.x = TILESET_DEFAULT_SIZE;
    // q.dim.y = TILESET_DEFAULT_SIZE;
    // q.tc = TILESET_RARROW_TILE;
    // q.angle_rad = atan2(y_off, x_off);
    // nbatch2d_rend_add_quad(&gs.batch_rend, q, &gs.atlas);
}

void game_state_update_and_render() {
    do_game_gui();
    if (ninput_key_pressed(get_nim(), NKEY_SCANCODE_ESCAPE)) {game_state_status_set(GAME_STATUS_START_MENU);}

    if (game_state_status_match(GAME_STATUS_RUNNING)) {
        nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs.player, nPhysicsBody);
        vec2 player_pos = v2(b->position.x + b->dim.x/2, b->position.y + b->dim.y/2);
        nScrollAmount scroll_y = ninput_get_scroll_amount_delta(get_nim());
        if (scroll_y) { gs.dcam.zoom += scroll_y * 0.1; }
        //ndungeon_cam_update(&gs.dcam, v2(player_pos.x, player_pos.y));
        // ----
        nem_update(get_em());
    }

    if (ninput_key_pressed(get_nim(), NKEY_SCANCODE_SPACE)) {
        game_state_status_set(game_state_status_match(GAME_STATUS_PAUSED) ? GAME_STATUS_RUNNING : GAME_STATUS_PAUSED);
    }
    // if (ninput_key_pressed(get_nim(), NKEY_SCANCODE_EQUALS)) {nactor_cm_gc(&gs.acm);}
    // if (game_state_status_match(GAME_STATUS_START_MENU)) {
    //     mat4 viewm = m4d(1);
    //     nbatch2d_rend_set_view_mat(&gs.batch_rend, viewm);
    //     nbatch2d_rend_begin(&gs.batch_rend, get_nwin);
    //     game_state_render_silly_stuff();
    //     nbatch2d_rend_end(&gs.batch_rend);
    //     return;
    // }

    // ivec2 player_pos = iv2(0,0);
    // nActorComponent *player_cmp = nactor_cm_get(&(gs.acm), gs.map.player);
    // if (player_cmp) {
    //     nmap_compute_fov(&(gs.map), player_cmp->posx, player_cmp->posy, 3);
    //     player_pos = iv2(player_cmp->posx, player_cmp->posy);
    // }

    // // If game is running, we can simulate!
    // if (game_state_status_match(GAME_STATUS_RUNNING)) {
    //     nactor_cm_simulate(&(gs.acm), &(gs.map), game_state_status_match(GAME_STATUS_RUNNING));
    // }
    // // If game paused, do editor stuff (TODO -- we should have some form of tween-animation for this transition)
    // if (game_state_status_match(GAME_STATUS_PAUSED)) {
    //     vec2 mp = ninput_get_mouse_pos(get_nim());
    //     mp = ndungeon_screen_to_world(&gs.dcam, mp);
    //     ivec2 tile = iv2(floor(mp.x), floor(mp.y));
    //     nTile *t = nmap_tile_ref(&gs.map, tile.x, tile.y);
    //     if (t) {
    //         if (gui_input_mb_down(GUI_LMB)) {
    //             nmap_dig_region(&gs.map, tile.x, tile.y, tile.x+1, tile.y+1, NTILE_KIND_WALL);
    //         }
    //         if (gui_input_mb_down(GUI_RMB)) {
    //             nmap_dig_region(&gs.map, tile.x, tile.y, tile.x+1, tile.y+1, NTILE_KIND_GROUND);
    //         }
    //     }
    // }

    // if (game_state_status_match(GAME_STATUS_RUNNING) || game_state_status_match(GAME_STATUS_PAUSED)) {
    //     nScrollAmount scroll_y = ninput_get_scroll_amount_delta(get_nim());
    //     if (scroll_y) { gs.dcam.zoom += scroll_y * 0.1; }
    //     ndungeon_cam_update(&gs.dcam, v2(player_pos.x, player_pos.y));
    //     mat4 view = ndungeon_cam_get_view_mat(&gs.dcam);
    //     nbatch2d_rend_set_view_mat(&gs.batch_rend, view);
    //     nbatch2d_rend_begin(&gs.batch_rend, get_nwin());
    //     nmap_render(&(gs.map), &(gs.batch_rend), &(gs.atlas));
    //     game_state_render_dir_arrow(v2(player_pos.x,player_pos.y));
    //     if (game_state_status_match(GAME_STATUS_PAUSED)) {
    //         game_state_render_selection_box();
    //     }
    //     nactor_cm_render(&(gs.acm), &(gs.batch_rend), &(gs.atlas));
    //     nbatch2d_rend_end(&gs.batch_rend);
    // }
}

void game_state_status_set(GameStatus status) {
    gs.status = status;
}

b32  game_state_status_match(GameStatus status) {
    return (gs.status == status);
}

void game_state_generate_new_level() {
    //nactor_cm_clear(&(get_ggs()->acm));
    //nmap_create_ex(&gs.map, 64,64, 8,0.4,0.6);
}